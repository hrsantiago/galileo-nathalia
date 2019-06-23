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

void tests::beam::static_nonlinear::inelastic::steel::cantilever_tip_force(void) 
{
	/*
	fea::mesh::materials::Steel cantilever beam subjected to a tip force
	Analytical solution:
	---Elastic limit load---
	Pe = sy b h^2 / 6L
	---Plastic limit load---
	Pp = sy b h^2 / 4L
	 */

	//model
	fea::models::Model model("cantilever tip force", "benchmarks/beam/static/nonlinear/inelastic/steel");

	//nodes
	model.mesh()->add_node(0, 0, 0);
	model.mesh()->add_node(1, 0, 0);
	
	//cells
	model.mesh()->add_cell(fea::mesh::cells::type::beam);

	//materials
	model.mesh()->add_material(fea::mesh::materials::type::steel);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->yield_stress(250e6);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->break_stress(251e6);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->elastic_modulus(200e9);

	//sections
	model.mesh()->add_section(fea::mesh::sections::type::rectangle);
	((fea::mesh::sections::Rectangle*) model.mesh()->section(0))->mesh_w(20);
	((fea::mesh::sections::Rectangle*) model.mesh()->section(0))->mesh_h(20);
	((fea::mesh::sections::Rectangle*) model.mesh()->section(0))->width(0.10);
	((fea::mesh::sections::Rectangle*) model.mesh()->section(0))->height(0.10);

	//elements
	model.mesh()->add_element(fea::mesh::elements::type::beam3, {0, 1});
	
	//refine
	fea::mesh::cells::Line::refine(0, 5);

	//supports
	model.boundary()->add_support(0, fea::mesh::nodes::dof::rotation_x);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::rotation_y);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::rotation_z);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_y);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_z);

	//loads
	model.boundary()->add_load_case(1, fea::mesh::nodes::dof::translation_y, -62.5e3);

	//solver
	fea::mesh::elements::Beam2::shear(false);
	fea::mesh::elements::Beam2::mixed(false);
	fea::mesh::elements::Mechanic::geometric(true);
	fea::mesh::elements::Mechanic::inelastic(false);
	model.analysis()->solver(fea::analysis::solvers::type::static_nonlinear);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->step_max(80);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_max(2.0);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_predictor(0.10);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->watch_dof(1, fea::mesh::nodes::dof::translation_y);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->strategy(fea::analysis::strategies::type::arc_length_cylindric);

	//solve
	model.analysis()->solve();
	
	//save
	model.save();
}
