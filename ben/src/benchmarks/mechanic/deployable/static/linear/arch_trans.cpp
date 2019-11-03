//std
#include <cmath>

//mat
#include "linear/dense.h"
#include "linear/linear.h"

//fea
#include "Model/Model.h"

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
const static bool grav = false;		//self weight

//geometry
const static double S = 2.00e+00;	//span
const static double H = 1.00e-00;	//height
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
const static double Pr = 1.00e+00;	//load reference value

const static unsigned nr = 1;		//units elements
const static unsigned nu = 10;		//number of units

//data
static double ps[nu + 1], pr[nu], ds[nu], pm[nu * nu];

//parameters
const static double f = S / H / 2;
const static double R = H / 2 * (1 + f * f);
const static double q = 2 * asin(2 * f / (1 + f * f));

const static double qi = (M_PI - q) / 2;
const static double qj = (M_PI + q) / 2;

const static unsigned nw = nu % 2 == 0 ? 3 * nu + 3 : 3 * nu - 3;

//curve
static double xc(double t)
{
	return R * cos(t);
}
static double yc(double t)
{
	return R * sin(t);
}
static double dx(double t)
{
	return -R * sin(t);
}
static double dy(double t)
{
	return +R * cos(t);
}

//points
static bool points(void)
{
	//predictor
	double a = t / 2;
	double b = sqrt(t * t + a * a);
	for(unsigned i = 0; i <= nu; i++)
	{
		ps[i] = (qj - qi) * i / nu + qi;
	}
	//corrector
	for(unsigned i = 0; i < 20; i++)
	{
		//check
		for(unsigned j = 0; j < nu; j++)
		{
			const double xi = xc(ps[j + 0]);
			const double xj = xc(ps[j + 1]);
			const double yi = yc(ps[j + 0]);
			const double yj = yc(ps[j + 1]);
			pr[j] = pow(xj - xi, 2) / (a * a) + pow(yj - yi, 2) / (b * b) - 1;
		}
		const double e = mat::norm(pr, nu);
		printf("\titeration: %02d error: %+.2e\n", i, e);
		if(e < 1e-5)
		{
			return true;
		}
		//update
		mat::clean(pm, nu * nu);
		for(unsigned j = 0; j < nu; j++)
		{
			const double xi = xc(ps[j + 0]);
			const double xj = xc(ps[j + 1]);
			const double yi = yc(ps[j + 0]);
			const double yj = yc(ps[j + 1]);
			if(j != 0)
			{
				pm[j + nu * (j - 1)] = -2 * (xj - xi) * dx(ps[j + 0]) / (a * a) - 2 * (yj - yi) * dy(ps[j + 0]) / (b * b);
			}
			if(j != nu - 1)
			{
				pm[j + nu * (j + 0)] = +2 * (xj - xi) * dx(ps[j + 1]) / (a * a) + 2 * (yj - yi) * dy(ps[j + 1]) / (b * b);
			}
			pm[j + nu * (nu - 1)] = -2 * pow(xj - xi, 2) / (a * a * a) - 2 * pow(yj - yi, 2) / (b * b * b) * (a / b);
		}
		mat::solve(ds, pm, pr, nu);
		for(unsigned j = 0; j < nu - 1; j++)
		{
			ps[j + 1] -= ds[j];
		}
		a -= ds[nu - 1];
		b = sqrt(a * a + t * t);
	}
	//return
	return false;
}

//model
void tests::deployable::static_linear::arch_trans(void)
{
	//model
	fea::models::Model model("arch trans", "benchmarks/deployable/static/linear");
	
	//points
	if(!points())
	{
		printf("\tError: Could not calculate points!\n");
		return;
	}
	
	//nodes
	for(unsigned i = 0; i < nu; i++)
	{
		const double xi = xc(ps[i + 0]);
		const double xj = xc(ps[i + 1]);
		const double yi = yc(ps[i + 0]);
		const double yj = yc(ps[i + 1]);
		const double xm = (xi + xj) / 2;
		const double ym = (yi + yj) / 2;
		model.mesh()->add_node(xm, ym, 0);
		model.mesh()->add_node(xm, ym, 0);
		model.mesh()->add_node(xi, yi - t / 2, 0);
		model.mesh()->add_node(xi, yi + t / 2, 0);
		model.mesh()->add_node(xj, yj - t / 2, 0);
		model.mesh()->add_node(xj, yj + t / 2, 0);
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
		model.mesh()->add_element(fea::mesh::elements::type::beam2, {6 * i + 0, 6 * i + 2}, { 0 }, 1);
		model.mesh()->add_element(fea::mesh::elements::type::beam2, {6 * i + 0, 6 * i + 5}, { 0 }, 1);
		model.mesh()->add_element(fea::mesh::elements::type::beam2, {6 * i + 1, 6 * i + 3}, { 0 }, 1);
		model.mesh()->add_element(fea::mesh::elements::type::beam2, {6 * i + 1, 6 * i + 4}, { 0 }, 1);
	}
	
	//joints
	for(unsigned i = 0; i < nu; i++)
	{
		model.mesh()->add_joint(fea::mesh::joints::type::pinned, {6 * i + 0, 6 * i + 1});
	}
	for(unsigned i = 0; i + 1 < nu; i++)
	{
		model.mesh()->add_joint(fea::mesh::joints::type::pinned, {6 * i + 4, 6 * (i + 1) + 2});
		model.mesh()->add_joint(fea::mesh::joints::type::pinned, {6 * i + 5, 6 * (i + 1) + 3});
	}
	for(unsigned i = 0; i < 3 * nu - 2; i++)
	{
		model.mesh()->joint(i)->mass(m);
	}
	
	//supports
	model.boundary()->add_support(6 * 0 + 2, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(6 * 0 + 2, fea::mesh::nodes::dof::translation_y);
	model.boundary()->add_support(6 * 0 + 3, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(6 * 0 + 3, fea::mesh::nodes::dof::translation_y);
	model.boundary()->add_support(6 * (nu - 1) + 4, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(6 * (nu - 1) + 4, fea::mesh::nodes::dof::translation_y);
	model.boundary()->add_support(6 * (nu - 1) + 5, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(6 * (nu - 1) + 5, fea::mesh::nodes::dof::translation_y);
	
	//load cases
	model.boundary()->add_load_case("Deployment");
	model.boundary()->add_self_weight("Gravity", fea::mesh::nodes::dof::translation_y);
	for(unsigned i = 0; i < nu - 1; i++)
	{
		model.boundary()->load_case(0)->add_load_node(6 * i + 4, fea::mesh::nodes::dof::translation_y, -Pr);
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
