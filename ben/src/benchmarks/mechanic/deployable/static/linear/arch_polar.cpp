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
#include "Mesh/Materials/Mechanic/Associative/Steel.h"

#include "Boundary/Boundary.h"
#include "Boundary/Cases/Load_Case.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Types.h"
#include "Analysis/Solvers/Static_Linear.h"

//ben
#include "benchmarks/mechanic/deployable.h"

//controls
const static bool grav = true;		//self weight

//geometry
const static double S = 6.00e+00;	//span
const static double H = 3.00e-00;	//height
const static double t = 6.00e-01;	//thickness

//cross section
const static double w = 1.00e-01;	//width
const static double h = 1.00e-01;	//height
const static double a = 5.00e-03;	//thickness

//material
const static double m = 0.00e+00;	//joint mass
const static double r = 2.75e+03;	//specific mass
const static double E = 7.00e+10;	//elastic modulus

//analysis
const static double Pr = 1.89e+04;	//load reference value

const static unsigned nr = 1;		//units elements
const static unsigned nu = 4;		//number of units

//parameters
const static double f = S / H / 2;
const static double R = H / 2 * (1 + f * f);
const static double q = 2 * asin(2 * f / (1 + f * f));

const static double dq = q / nu / 2;
const static double q0 = (M_PI - q) / 2;

const static double b = (R - t * cos(dq) + sqrt(pow(R, 2) + pow(t * cos(dq), 2))) / (2 * cos(dq));

const static unsigned nw = nu % 2 == 0 ? 6 * (nu / 2 - 1) + 5 : 6 * (nu / 2) + 0;

//model
void tests::deployable::static_linear::arch_polar(void)
{
	//model
	fea::models::Model model("arch polar", "benchmarks/deployable/static/linear");
	
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
	model.mesh()->add_cell(fea::mesh::cells::type::beam);

	//materials
	model.mesh()->add_material(fea::mesh::materials::type::steel);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->specific_mass(r);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->elastic_modulus(E);

	//sections
	model.mesh()->add_section(fea::mesh::sections::type::box);
	((fea::mesh::sections::Box*) model.mesh()->section(0))->width(w);
	((fea::mesh::sections::Box*) model.mesh()->section(0))->height(h);
	((fea::mesh::sections::Box*) model.mesh()->section(0))->thickness(a);
	
	//elements
	for(unsigned i = 0; i < nu; i++)
	{
		model.mesh()->add_element(fea::mesh::elements::type::beam2, {6 * i + 0, 6 * i + 2});
		model.mesh()->add_element(fea::mesh::elements::type::beam2, {6 * i + 0, 6 * i + 5});
		model.mesh()->add_element(fea::mesh::elements::type::beam2, {6 * i + 1, 6 * i + 3});
		model.mesh()->add_element(fea::mesh::elements::type::beam2, {6 * i + 1, 6 * i + 4});
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
	model.boundary()->add_support(6 * 0 + 4, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(6 * 0 + 4, fea::mesh::nodes::dof::translation_y);
	model.boundary()->add_support(6 * (nu - 1) + 3, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(6 * (nu - 1) + 3, fea::mesh::nodes::dof::translation_y);
	model.boundary()->add_support(6 * (nu - 1) + 5, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(6 * (nu - 1) + 5, fea::mesh::nodes::dof::translation_y);
	
	//load cases
	model.boundary()->add_load_case("Deployment");
	model.boundary()->add_self_weight("Gravity", fea::mesh::nodes::dof::translation_y);
	for(unsigned i = 0; i < nu - 1; i++)
	{
		model.boundary()->load_case(0)->add_load_node(6 * i + 3, fea::mesh::nodes::dof::translation_y, -Pr / (nu - 1));
	}
	
	//solver
	model.analysis()->solver(fea::analysis::solvers::type::static_linear);
	model.analysis()->solver()->watch_dof(nw, fea::mesh::nodes::dof::translation_y);
	dynamic_cast<fea::analysis::solvers::Static_Linear*> (model.analysis()->solver())->dead(grav);
	dynamic_cast<fea::analysis::solvers::Static_Linear*> (model.analysis()->solver())->dead_case(1);
	dynamic_cast<fea::analysis::solvers::Static_Linear*> (model.analysis()->solver())->load_case(0);
	
	//solve
	model.analysis()->solve();

	//save
	model.save();
}
