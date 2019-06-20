//std
#include <cmath>

//fea
#include "Models/Model.h"

#include "Mesh/Mesh.h"
#include "Mesh/Nodes/Dofs.h"
#include "Mesh/Cells/Types.h"
#include "Mesh/Sections/Types.h"
#include "Mesh/Elements/Types.h"
#include "Mesh/Cells/Line/Line.h"
#include "Mesh/Materials/Types.h"
#include "Mesh/Sections/Rectangle.h"
#include "Mesh/Elements/Mechanic/Mechanic.h"
#include "Mesh/Elements/Mechanic/Frame/Beam2.h"
#include "Mesh/Materials/Mechanic/Associative/Steel.h"

#include "Boundary/Boundary.h"
#include "Boundary/Cases/Load_Case.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Types.h"
#include "Analysis/Strategies/Types.h"
#include "Analysis/Solvers/Static_Nonlinear.h"

//ben
#include "benchmarks/mechanic/beam.h"

void tests::beam::static_nonlinear::inelastic::steel::point_test_4(void) 
{
	/*
	Steel pinned beam four point test
	Literature: H. B. Coda and R. B. Paccola (2014) - Finite Elements in Analysis and Design - pp. 1 - 15
	 */

	//model
	fea::models::Model model("point test 4", "benchmarks/beam/static/nonlinear/inelastic/steel");

	//nodes
	model.mesh()->add_node(0, 0, 0);
	model.mesh()->add_node(100, 0, 0);
	model.mesh()->add_node(200, 0, 0);
	model.mesh()->add_node(300, 0, 0);

	//cells
	model.mesh()->add_cell(fea::mesh::cells::type::beam);

	//materials
	model.mesh()->add_material(fea::mesh::materials::type::steel);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->yield_stress(2.00e1);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->break_stress(0.00e0);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->break_strain(6.00e-3);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->elastic_modulus(2.00e4);

	//sections
	model.mesh()->add_section(fea::mesh::sections::type::rectangle);
	((fea::mesh::sections::Rectangle*) model.mesh()->section(0))->mesh_w(1);
	((fea::mesh::sections::Rectangle*) model.mesh()->section(0))->mesh_h(80);
	((fea::mesh::sections::Rectangle*) model.mesh()->section(0))->width(9.00e0);
	((fea::mesh::sections::Rectangle*) model.mesh()->section(0))->height(9.00e0);

	//elements
	model.mesh()->add_element(fea::mesh::elements::type::beam2, {0, 1});
	model.mesh()->add_element(fea::mesh::elements::type::beam2, {1, 2});
	model.mesh()->add_element(fea::mesh::elements::type::beam2, {2, 3});

	//refine
	fea::mesh::cells::Line::refine(0, 5);
	fea::mesh::cells::Line::refine(1, 5);
	fea::mesh::cells::Line::refine(2, 5);

	//supports
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_y);
	model.boundary()->add_support(3, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(3, fea::mesh::nodes::dof::translation_y);

	//loads
	model.boundary()->add_load_case();
	model.boundary()->load_case(0)->add_load_node(1, fea::mesh::nodes::dof::translation_y, -3.645e1);
	model.boundary()->load_case(0)->add_load_node(2, fea::mesh::nodes::dof::translation_y, -3.645e1);

	//solver
	fea::mesh::elements::Beam2::mixed(false);
	fea::mesh::elements::Beam2::shear(false);
	fea::mesh::elements::Mechanic::inelastic(true);
	fea::mesh::elements::Mechanic::geometric(false);
	model.analysis()->solver(fea::analysis::solvers::type::static_nonlinear);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->step_max(200);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_max(1.1);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_predictor(0.05);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->watch_dof(1, fea::mesh::nodes::dof::translation_y);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->strategy(fea::analysis::strategies::type::minimal_norm);

	//solve
	model.analysis()->solve();
	
	//save
	model.save();
}
