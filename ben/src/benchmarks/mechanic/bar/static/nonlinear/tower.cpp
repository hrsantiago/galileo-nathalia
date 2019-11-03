//std
#include <cmath>

//fea
#include "Model/Model.h"

#include "Mesh/Mesh.h"
#include "Mesh/Nodes/Dofs.h"
#include "Mesh/Cells/Types.h"
#include "Mesh/Sections/Types.h"
#include "Mesh/Elements/Types.h"
#include "Mesh/Cells/Line/Line.h"
#include "Mesh/Sections/Circle.h"
#include "Mesh/Materials/Types.h"
#include "Mesh/Elements/Mechanic/Mechanic.h"
#include "Mesh/Elements/Mechanic/Frame/Bar.h"
#include "Mesh/Materials/Mechanic/Associative/Steel.h"

#include "Boundary/Boundary.h"
#include "Boundary/Cases/Load_Case.h"
#include "Boundary/Dependencies/Dependency.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Types.h"
#include "Analysis/Strategies/Types.h"
#include "Analysis/Solvers/Static_Nonlinear.h"

//ben
#include "benchmarks/mechanic/bar.h"

void tests::bar::static_nonlinear::tower(void)
{
	//parameters
	const bool strain = true;
	const double b = 4.00e+00;
	const double h = 2.00e+01;
	const double s = 2.50e+08;
	const double E = 2.10e+11;
	const double K = 2.10e+10;
	const double A1 = 3.50e-03;
	const double A2 = 7.50e-04;
	const double d1 = sqrt(4 * A1 / M_PI);
	const double d2 = sqrt(4 * A2 / M_PI);

	//model
	fea::models::Model model("tower", "benchmarks/bar/static/nonlinear");

	//nodes
	model.mesh()->add_node(0, 0, h);
	model.mesh()->add_node(-b / 2, -b / 2, 0);
	model.mesh()->add_node(+b / 2, -b / 2, 0);
	model.mesh()->add_node(+b / 2, +b / 2, 0);
	model.mesh()->add_node(-b / 2, +b / 2, 0);
	model.mesh()->add_node(-b / 4, -b / 4, h / 2);
	model.mesh()->add_node(+b / 4, -b / 4, h / 2);
	model.mesh()->add_node(+b / 4, +b / 4, h / 2);
	model.mesh()->add_node(-b / 4, +b / 4, h / 2);

	//cells
	model.mesh()->add_cell(fea::mesh::cells::type::bar);
	model.mesh()->add_cell(fea::mesh::cells::type::bar);
	((fea::mesh::cells::Line*) model.mesh()->cell(0))->section(0);
	((fea::mesh::cells::Line*) model.mesh()->cell(1))->section(1);

	//materials
	model.mesh()->add_material(fea::mesh::materials::type::steel);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->yield_stress(s);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->break_strain(1e30);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->elastic_modulus(E);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->plastic_modulus(K);

	//sections
	model.mesh()->add_section(fea::mesh::sections::type::circle);
	model.mesh()->add_section(fea::mesh::sections::type::circle);
	((fea::mesh::sections::Circle*) model.mesh()->section(0))->diameter(d1);
	((fea::mesh::sections::Circle*) model.mesh()->section(1))->diameter(d2);

	//elements
	model.mesh()->add_element(fea::mesh::elements::type::bar, {0, 5}, 0, 0);
	model.mesh()->add_element(fea::mesh::elements::type::bar, {0, 6}, 0, 0);
	model.mesh()->add_element(fea::mesh::elements::type::bar, {0, 7}, 0, 0);
	model.mesh()->add_element(fea::mesh::elements::type::bar, {0, 8}, 0, 0);
	model.mesh()->add_element(fea::mesh::elements::type::bar, {5, 1}, 0, 0);
	model.mesh()->add_element(fea::mesh::elements::type::bar, {6, 2}, 0, 0);
	model.mesh()->add_element(fea::mesh::elements::type::bar, {7, 3}, 0, 0);
	model.mesh()->add_element(fea::mesh::elements::type::bar, {8, 4}, 0, 0);
	model.mesh()->add_element(fea::mesh::elements::type::bar, {5, 6}, 0, 1);
	model.mesh()->add_element(fea::mesh::elements::type::bar, {6, 7}, 0, 1);
	model.mesh()->add_element(fea::mesh::elements::type::bar, {7, 8}, 0, 1);
	model.mesh()->add_element(fea::mesh::elements::type::bar, {8, 5}, 0, 1);
	model.mesh()->add_element(fea::mesh::elements::type::bar, {5, 2}, 0, 1);
	model.mesh()->add_element(fea::mesh::elements::type::bar, {6, 1}, 0, 1);
	model.mesh()->add_element(fea::mesh::elements::type::bar, {6, 3}, 0, 1);
	model.mesh()->add_element(fea::mesh::elements::type::bar, {7, 2}, 0, 1);
	model.mesh()->add_element(fea::mesh::elements::type::bar, {7, 4}, 0, 1);
	model.mesh()->add_element(fea::mesh::elements::type::bar, {8, 3}, 0, 1);
	model.mesh()->add_element(fea::mesh::elements::type::bar, {8, 1}, 0, 1);
	model.mesh()->add_element(fea::mesh::elements::type::bar, {5, 4}, 0, 1);

	//supports
	model.boundary()->add_support(1, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(1, fea::mesh::nodes::dof::translation_y);
	model.boundary()->add_support(1, fea::mesh::nodes::dof::translation_z);
	model.boundary()->add_support(2, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(2, fea::mesh::nodes::dof::translation_y);
	model.boundary()->add_support(2, fea::mesh::nodes::dof::translation_z);
	model.boundary()->add_support(3, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(3, fea::mesh::nodes::dof::translation_y);
	model.boundary()->add_support(3, fea::mesh::nodes::dof::translation_z);
	model.boundary()->add_support(4, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(4, fea::mesh::nodes::dof::translation_y);
	model.boundary()->add_support(4, fea::mesh::nodes::dof::translation_z);

	//loads
	model.boundary()->add_load_case();
	model.boundary()->load_case(0)->add_load_node(0, fea::mesh::nodes::dof::translation_z, -3.00e6);
	model.boundary()->load_case(0)->add_load_node(0, fea::mesh::nodes::dof::translation_x, +3.75e4);
	model.boundary()->load_case(0)->add_load_node(5, fea::mesh::nodes::dof::translation_x, +1.50e5);
	model.boundary()->load_case(0)->add_load_node(8, fea::mesh::nodes::dof::translation_x, +1.50e5);

	//solver
	fea::mesh::elements::Bar::strain(strain);
	fea::mesh::elements::Mechanic::geometric(true);
	fea::mesh::elements::Mechanic::inelastic(true);
	model.analysis()->solver(fea::analysis::solvers::type::static_nonlinear);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*>(model.analysis()->solver())->load_max(1);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*>(model.analysis()->solver())->step_max(2000);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*>(model.analysis()->solver())->load_predictor(0.01);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*>(model.analysis()->solver())->watch_dof(0, fea::mesh::nodes::dof::translation_x);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*>(model.analysis()->solver())->strategy(fea::analysis::strategies::type::minimal_norm);

	//solve
	model.analysis()->solve();

	//save
	model.save();
}
