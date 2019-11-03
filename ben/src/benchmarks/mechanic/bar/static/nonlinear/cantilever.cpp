//std
#include <cmath>

//fea
#include "Model/Model.h"

#include "Mesh/Mesh.h"
#include "Mesh/Nodes/Dofs.h"
#include "Mesh/Cells/Types.h"
#include "Mesh/Sections/Types.h"
#include "Mesh/Elements/Types.h"
#include "Mesh/Sections/Circle.h"
#include "Mesh/Materials/Types.h"
#include "Mesh/Elements/Mechanic/Mechanic.h"
#include "Mesh/Elements/Mechanic/Frame/Bar.h"
#include "Mesh/Materials/Mechanic/Associative/Steel.h"

#include "Boundary/Boundary.h"
#include "Boundary/Dependencies/Dependency.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Types.h"
#include "Analysis/Strategies/Types.h"
#include "Analysis/Solvers/Static_Nonlinear.h"

//ben
#include "benchmarks/mechanic/bar.h"

void tests::bar::static_nonlinear::cantilever(void)
{
	//parameters
	const unsigned n = 10;
	const bool strain = true;
	const double A = 6.45e-04;
	const double b = 5.08e-01;
	const double h = 5.08e-01;
	const double s = 2.07e+08;
	const double E = 2.00e+11;
	const double d = sqrt(4 * A / M_PI);

	//model
	fea::models::Model model("cantilever", "benchmarks/bar/static/nonlinear");

	//nodes
	for(unsigned i = 0; i <= n; i++)
	{
		model.mesh()->add_node(i * b, 0, 0);
		model.mesh()->add_node(i * b, h, 0);
	}

	//cells
	model.mesh()->add_cell(fea::mesh::cells::type::bar);

	//materials
	model.mesh()->add_material(fea::mesh::materials::type::steel);
	((fea::mesh::materials::Steel*)model.mesh()->material(0))->yield_stress(s);
	((fea::mesh::materials::Steel*)model.mesh()->material(0))->break_strain(1e30);
	((fea::mesh::materials::Steel*)model.mesh()->material(0))->elastic_modulus(E);
	((fea::mesh::materials::Steel*)model.mesh()->material(0))->plastic_modulus(E / 9);

	//sections
	model.mesh()->add_section(fea::mesh::sections::type::circle);
	((fea::mesh::sections::Circle*)model.mesh()->section(0))->diameter(d);

	//elements
	for(unsigned i = 0; i <= n; i++)
	{
		model.mesh()->add_element(fea::mesh::elements::type::bar, {2 * i, 2 * i + 1});
	}
	for(unsigned i = 0; i < n; i++)
	{
		model.mesh()->add_element(fea::mesh::elements::type::bar, {2 * (i + 0) + 0, 2 * (i + 1) + 0});
		model.mesh()->add_element(fea::mesh::elements::type::bar, {2 * (i + 0) + 1, 2 * (i + 1) + 1});
		model.mesh()->add_element(fea::mesh::elements::type::bar, {2 * (i + 0) + 0, 2 * (i + 1) + 1});
		model.mesh()->add_element(fea::mesh::elements::type::bar, {2 * (i + 0) + 1, 2 * (i + 1) + 0});
	}

	//supports
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_y);
	model.boundary()->add_support(1, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(1, fea::mesh::nodes::dof::translation_y);
	for(unsigned i = 0; i <= n; i++)
	{
		model.boundary()->add_support(2 * i + 0, fea::mesh::nodes::dof::translation_z);
		model.boundary()->add_support(2 * i + 1, fea::mesh::nodes::dof::translation_z);
	}

	//loads
	model.boundary()->add_load_case(2 * n + 0, fea::mesh::nodes::dof::translation_y, -1e3);

	//solver
	fea::mesh::elements::Bar::strain(strain);
	fea::mesh::elements::Mechanic::geometric(true);
	fea::mesh::elements::Mechanic::inelastic(true);
	model.analysis()->solver(fea::analysis::solvers::type::static_nonlinear);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*>(model.analysis()->solver())->load_max(400);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*>(model.analysis()->solver())->step_max(2000);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*>(model.analysis()->solver())->load_predictor(2.0);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*>(model.analysis()->solver())->strategy(fea::analysis::strategies::type::control_state);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*>(model.analysis()->solver())->watch_dof(2 * n + 0, fea::mesh::nodes::dof::translation_y);

	//solve
	model.analysis()->solve();

	//save
	model.save();
}
