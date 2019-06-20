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
#include "Mesh/Materials/Mechanic/Associative/Steel.h"

#include "Boundary/Boundary.h"
#include "Boundary/Loads/Types.h"
#include "Boundary/Cases/Load_Case.h"
#include "Boundary/Loads/Elements/Element.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Types.h"
#include "Analysis/Strategies/Types.h"
#include "Analysis/Solvers/Static_Nonlinear.h"

//ben
#include "benchmarks/mechanic/beam.h"

void tests::beam::static_nonlinear::inelastic::steel::propped_force_distributed(void) 
{
	/*
	fea::mesh::materials::Steel propped beam subjected to a distributed force
	Literature: D. W. A. Rees - Mechanics of Optimal Structural Design (2009) pp. 477
	 */

	//model
	fea::models::Model model("propped force distributed", "benchmarks/beam/static/nonlinear/inelastic/steel");

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
	((fea::mesh::sections::Rectangle*) model.mesh()->section(0))->mesh_w(1);
	((fea::mesh::sections::Rectangle*) model.mesh()->section(0))->mesh_h(80);
	((fea::mesh::sections::Rectangle*) model.mesh()->section(0))->width(0.10);
	((fea::mesh::sections::Rectangle*) model.mesh()->section(0))->height(0.10);

	//elements
	model.mesh()->add_element(fea::mesh::elements::type::beam2, {0, 1});

	//supports
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_y);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::rotation_z);
	model.boundary()->add_support(1, fea::mesh::nodes::dof::translation_y);

	//loads
	model.boundary()->add_load_case();
	model.boundary()->load_case(0)->add_load_element(fea::boundary::loads::type::shear);
	model.boundary()->load_case(0)->load_element(0)->value(-726.74e3);
	for(unsigned i = 0; i < 10; i++)
	{
		model.boundary()->load_case(0)->load_element(0)->add_element(i);
	}

	//solver
	fea::mesh::elements::Mechanic::geometric(true);
	fea::mesh::elements::Mechanic::inelastic(true);
	model.analysis()->solver(fea::analysis::solvers::type::static_nonlinear);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->step_max(80);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_max(1.2);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_predictor(0.10);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->strategy(fea::analysis::strategies::type::arc_length_cylindric);

	//solve
	model.analysis()->solve();
	
	//save
	model.save();
}
