//std
#include <cmath>

//mat
#include "misc/defs.h"

//fea
#include "Models/Model.h"

#include "Mesh/Mesh.h"
#include "Mesh/Nodes/Dofs.h"
#include "Mesh/Cells/Types.h"
#include "Mesh/Joints/Types.h"
#include "Mesh/Joints/Joint.h"
#include "Mesh/Sections/Box.h"
#include "Mesh/Sections/Types.h"
#include "Mesh/Elements/Types.h"
#include "Mesh/Cells/Line/Line.h"
#include "Mesh/Sections/Circle.h"
#include "Mesh/Materials/Types.h"
#include "Mesh/Sections/Rectangle.h"
#include "Mesh/Elements/Mechanic/Mechanic.h"
#include "Mesh/Materials/Mechanic/Associative/Steel.h"

#include "Boundary/Boundary.h"
#include "Boundary/Cases/Load_Case.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Types.h"
#include "Analysis/Strategies/Types.h"
#include "Analysis/Solvers/Static_Nonlinear.h"

//ben
#include "benchmarks/mechanic/deployable.h"

//controls
const static bool base = false;		//has cable
const static bool unif = true;		//uniform load
const static bool depl = false;		//solve deployment
const static bool grav = false;		//solve self weight

//geometry
const static double S = 2.00e+00;	//span
const static double H = 3.00e-01;	//height
const static double t = 1.00e-01;	//thickness

//cross section
const static double w = 5.00e-02;	//width
const static double h = 5.00e-02;	//height
const static double a = 5.00e-03;	//thickness
const static double d = 1.00e-01;	//cable diameter

//material
const static double m = 0.00e+00;	//joint mass
const static double r = 7.85e+03;	//specific mass
const static double E = 2.00e+11;	//elastic modulus

//analysis
const static double tl = 1.00e-03;	//tolerance
const static double Pl = 2.50e+06;	//load limit
const static double Pr = 1.00e+00;	//load reference value
const static double wr = 1.00e+00;	//uniform load reference
const static double dP = 1.00e+01;	//load initial increment

const static unsigned ns = 800;		//max steps
const static unsigned nm = 10;		//max iterations
const static unsigned nu = 10;		//number of units
const static unsigned nr = 1;		//units refinement
const static unsigned nd = 5;		//iterations desired

//parameters
const static double f = S / H / 2;
const static double R = H / 2 * (1 + f * f);
const static double q = 2 * asin(2 * f / (1 + f * f));

const static double dq = q / nu / 2;
const static double q0 = (M_PI - q) / 2;

const static double b = (R - t * cos(dq) + sqrt(pow(R, 2) + pow(t * cos(dq), 2))) / (2 * cos(dq));

const static unsigned nw = nu % 2 == 0 ? 6 * (nu / 2 - 1) + 5 : 6 * (nu / 2) + 0;

//model
void tests::deployable::static_nonlinear::arch_polar(void)
{
	//model
	fea::models::Model model("arch polar", "benchmarks/deployable/static/nonlinear");
	
	//nodes
	for(unsigned i = 0; i < nu; i++)
	{
		const double q = q0 + (2 * i + 1) * dq;
		model.mesh()->add_node(R * cos(q), R * sin(q), 0);
		model.mesh()->add_node(R * cos(q), R * sin(q), 0);
		model.mesh()->add_node(b * cos(q - dq), b * sin(q - dq), 0);
		model.mesh()->add_node(b * cos(q + dq), b * sin(q + dq), 0);
		model.mesh()->add_node((b + t) * cos(q - dq), (b + t) * sin(q - dq), 0);
		model.mesh()->add_node((b + t) * cos(q + dq), (b + t) * sin(q + dq), 0);
	}
	
	//cells
	model.mesh()->add_cell(fea::mesh::cells::type::bar);
	model.mesh()->add_cell(fea::mesh::cells::type::beam);
	((fea::mesh::cells::Line*) model.mesh()->cell(0))->section(1);
	((fea::mesh::cells::Line*) model.mesh()->cell(1))->section(0);

	//materials
	model.mesh()->add_material(fea::mesh::materials::type::steel);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->specific_mass(r);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->elastic_modulus(E);

	//sections
	model.mesh()->add_section(fea::mesh::sections::type::box);
	model.mesh()->add_section(fea::mesh::sections::type::circle);
	((fea::mesh::sections::Box*) model.mesh()->section(0))->width(w);
	((fea::mesh::sections::Box*) model.mesh()->section(0))->height(h);
	((fea::mesh::sections::Box*) model.mesh()->section(0))->thickness(a);
	((fea::mesh::sections::Circle*) model.mesh()->section(1))->diameter(d);
	
	//elements
	for(unsigned i = 0; i < nu; i++)
	{
		model.mesh()->add_element(fea::mesh::elements::type::beam2, {6 * i + 0, 6 * i + 2}, 0, 1);
		model.mesh()->add_element(fea::mesh::elements::type::beam2, {6 * i + 0, 6 * i + 5}, 0, 1);
		model.mesh()->add_element(fea::mesh::elements::type::beam2, {6 * i + 1, 6 * i + 3}, 0, 1);
		model.mesh()->add_element(fea::mesh::elements::type::beam2, {6 * i + 1, 6 * i + 4}, 0, 1);
	}
	if(base)
	{
		model.mesh()->add_element(fea::mesh::elements::type::bar, {6 * 0 + 0, 6 * (nu - 1) + 1}, 0, 0);
	}
	
	//sizes
	for(unsigned i = 0; i < nu; i++)
	{
		
		const double l0 = model.mesh()->cell(1)->edge(model.mesh()->element(4 * i + 0), 0);
		const double l1 = model.mesh()->cell(1)->edge(model.mesh()->element(4 * i + 1), 0);
		const double l2 = model.mesh()->cell(1)->edge(model.mesh()->element(4 * i + 2), 0);
		const double l3 = model.mesh()->cell(1)->edge(model.mesh()->element(4 * i + 3), 0);
		printf("unit: %02d sizes: (%.2f, %.2f)\n", i, l0 + l1, l2 + l3);
	}
	
	//refine
	for(unsigned i = 0; i < 4 * nu; i++)
	{
		fea::mesh::cells::Line::refine(i, nr);
	}
	
	//joints
	for(unsigned i = 0; i < nu; i++)
	{
		model.mesh()->add_joint(fea::mesh::joints::type::pinned, {6 * i + 0, 6 * i + 1});
	}
	for(unsigned i = 0; i + 1 < nu; i++)
	{
		model.mesh()->add_joint(fea::mesh::joints::type::pinned, {6 * i + 3, 6 * (i + 1) + 2});
		model.mesh()->add_joint(fea::mesh::joints::type::pinned, {6 * i + 5, 6 * (i + 1) + 4});
	}
	for(unsigned i = 0; i < 3 * nu - 2; i++)
	{
		model.mesh()->joint(i)->mass(m);
	}
	
	//supports
	model.boundary()->add_support(6 * 0 + 2, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(6 * 0 + 2, fea::mesh::nodes::dof::translation_y);
	model.boundary()->add_support(6 * 0 + 2, fea::mesh::nodes::dof::translation_z);
	model.boundary()->add_support(6 * (nu - 1) + 3, fea::mesh::nodes::dof::translation_y);
	model.boundary()->add_support(6 * (nu - 1) + 3, fea::mesh::nodes::dof::translation_z);
	if(!base)
	{
		model.boundary()->add_support(6 * (nu - 1) + 3, fea::mesh::nodes::dof::translation_x);
	}
	
	//load cases
	model.boundary()->add_load_case("Deployment");
	model.boundary()->add_self_weight("Gravity", fea::mesh::nodes::dof::translation_y);
	if(unif)
	{
		for(unsigned i = 0; i < nu; i++)
		{
			const double q = q0 + (2 * i + 1) * dq;
			const double s = (b + t) * fabs(cos(q + dq) - cos(q - dq));
			model.boundary()->load_case(0)->add_load_node(6 * i + 4, fea::mesh::nodes::dof::translation_y, -wr * s / 2);
			model.boundary()->load_case(0)->add_load_node(6 * i + 5, fea::mesh::nodes::dof::translation_y, -wr * s / 2);
		}
	}
	else
	{
		model.boundary()->load_case(0)->add_load_node(nw, fea::mesh::nodes::dof::translation_y, -Pr);
	}
	
	//solver
	fea::mesh::elements::Mechanic::geometric(true);
	model.analysis()->solver(fea::analysis::solvers::type::static_nonlinear);
	model.analysis()->solver()->watch_dof(nw, fea::mesh::nodes::dof::translation_y);
	
	//solve gravity
	if(grav)
	{
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_case(1);
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->step_max(100);
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_max(1e0);
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_predictor(5e-2);
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->strategy(fea::analysis::strategies::type::control_load);
		model.analysis()->solve();
	}
	
	//solve deployment
	if(depl)
	{
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->dead(grav);
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->dead_case(1);
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_case(0);
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->step_max(ns);
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_max(Pl);
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->tolerance(tl);
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->dof_min(-2 * H);
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->iteration_max(nm);
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_adjust(true);
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_predictor(dP);
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->iteration_desired(nd);
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->strategy(fea::analysis::strategies::type::control_state);
		model.analysis()->solve(grav);
	}

	//save
	model.save();
}
