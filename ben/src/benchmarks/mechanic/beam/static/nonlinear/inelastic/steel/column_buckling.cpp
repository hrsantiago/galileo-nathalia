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
#include "Mesh/Materials/Mechanic/Associative/Steel.h"

#include "Boundary/Boundary.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Types.h"
#include "Analysis/Strategies/Types.h"
#include "Analysis/Solvers/Static_Nonlinear.h"

//ben
#include "benchmarks/mechanic/beam.h"

void tests::beam::static_nonlinear::inelastic::steel::column_buckling(void)
{
	/*
	Steel column buckling
	Literature: H. B. Coda and R. B. Paccola (2014) - Finite Elements in Analysis and Design - pp. 1 - 15
	 */

	//model
	fea::models::Model model("column buckling", "benchmarks/beam/static/nonlinear/inelastic/steel");

	//nodes
	model.mesh()->add_node(0.00e+0, 0.00e+0, 0.00e+0);
	model.mesh()->add_node(0.00e+0, 3.00e+2, 0.00e+0);

	//cells
	model.mesh()->add_cell(fea::mesh::cells::type::beam);

	//materials
	model.mesh()->add_material(fea::mesh::materials::type::steel);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->yield_stress(2.10e1);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->break_stress(2.11e1);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->break_strain(1.00e-1);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->elastic_modulus(2.10e4);

	//sections
	model.mesh()->add_section(fea::mesh::sections::type::rectangle);
	((fea::mesh::sections::Rectangle*) model.mesh()->section(0))->mesh_w(1);
	((fea::mesh::sections::Rectangle*) model.mesh()->section(0))->mesh_h(80);
	((fea::mesh::sections::Rectangle*) model.mesh()->section(0))->width(3.00e0);
	((fea::mesh::sections::Rectangle*) model.mesh()->section(0))->height(3.00e0);

	//elements
	model.mesh()->add_element(fea::mesh::elements::type::beam2, {0, 1});

	//refine
	fea::mesh::cells::Line::refine(0, 10);

	//supports
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_y);
	model.boundary()->add_support(1, fea::mesh::nodes::dof::translation_x);

	//loads
	model.boundary()->add_load_case(1, fea::mesh::nodes::dof::translation_y, -1.5545e1);

	//solver
	fea::mesh::elements::Mechanic::geometric(true);
	fea::mesh::elements::Mechanic::inelastic(true);
	model.analysis()->solver(fea::analysis::solvers::type::static_nonlinear);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->step_max(500);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_max(3.0);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->branch_switch(true);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->mode_injection(100);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_predictor(0.1);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->watch_dof(6, fea::mesh::nodes::dof::translation_x);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->strategy(fea::analysis::strategies::type::arc_length_cylindric);

	//solve
	model.analysis()->solve();
	
	//save
	model.save();
}
