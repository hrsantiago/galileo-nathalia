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

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Types.h"
#include "Analysis/Strategies/Types.h"
#include "Analysis/Solvers/Static_Nonlinear.h"

//ben
#include "benchmarks/mechanic/beam.h"

void tests::beam::static_nonlinear::inelastic::steel::point_test_3(void) 
{
	/*
	Steel pinned beam three point test
	Literature: O. C. Zienkiewicz, R.L. (2000) pp. 304
	 */

	//model
	fea::models::Model model("point test 3", "benchmarks/beam/static/nonlinear/inelastic/steel");

	//nodes
	model.mesh()->add_node(0, 0, 0);
	model.mesh()->add_node(1, 0, 0);
	model.mesh()->add_node(2, 0, 0);
	
	//cells
	model.mesh()->add_cell(fea::mesh::cells::type::beam);

	//materials
	model.mesh()->add_material(fea::mesh::materials::type::steel);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->yield_stress(250e6);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->break_stress(251e6);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->elastic_modulus(200e9);

	//sections
	model.mesh()->add_section(fea::mesh::sections::type::rectangle);
	((fea::mesh::sections::Rectangle*) model.mesh()->section(0))->mesh_w(1);
	((fea::mesh::sections::Rectangle*) model.mesh()->section(0))->mesh_h(80);
	((fea::mesh::sections::Rectangle*) model.mesh()->section(0))->width(0.10);
	((fea::mesh::sections::Rectangle*) model.mesh()->section(0))->height(0.10);

	//elements
	model.mesh()->add_element(fea::mesh::elements::type::beam2, {0, 1});
	model.mesh()->add_element(fea::mesh::elements::type::beam2, {1, 2});

	//refine
	fea::mesh::cells::Line::refine(0, 5);
	fea::mesh::cells::Line::refine(1, 5);

	//supports
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_y);
	model.boundary()->add_support(2, fea::mesh::nodes::dof::translation_y);

	//loads
	model.boundary()->add_load_case(1, fea::mesh::nodes::dof::translation_y, -125e3);

	//solver
	fea::mesh::elements::Beam2::mixed(false);
	fea::mesh::elements::Beam2::shear(false);
	fea::mesh::elements::Mechanic::geometric(true);
	fea::mesh::elements::Mechanic::inelastic(true);
	model.analysis()->solver(fea::analysis::solvers::type::static_nonlinear);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->step_max(50);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_max(1.25);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_predictor(0.1);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->watch_dof(1, fea::mesh::nodes::dof::translation_y);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->strategy(fea::analysis::strategies::type::control_state);

	//solve
	model.analysis()->solve();
	
	//save
	model.save();
}
