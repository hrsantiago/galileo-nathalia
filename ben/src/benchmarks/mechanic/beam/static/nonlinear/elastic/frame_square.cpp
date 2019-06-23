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

void tests::beam::static_nonlinear::elastic::frame_square(void) 
{
	/*
	Square frame
	Literature: Chalmer University of Tecnology, Dep. of Structural Mechanics - K. Mattiasson (1981) pp. 149
	 */

	//model
	fea::models::Model model("frame square", "benchmarks/beam/static/nonlinear/elastic");

	//nodes
	model.mesh()->add_node(0, 0, 0);
	model.mesh()->add_node(0, 1, 0);
	model.mesh()->add_node(1, 1, 0);

	//cells
	model.mesh()->add_cell(fea::mesh::cells::type::beam);
	
	//materials
	model.mesh()->add_material(fea::mesh::materials::type::steel);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->elastic_modulus(100e9);

	//sections
	model.mesh()->add_section(fea::mesh::sections::type::rectangle);
	((fea::mesh::sections::Rectangle*) model.mesh()->section(0))->width(0.06);
	((fea::mesh::sections::Rectangle*) model.mesh()->section(0))->height(0.10);

	//elements
	fea::mesh::elements::Mechanic::geometric(true);
	model.mesh()->add_element(fea::mesh::elements::type::beam2, { 0, 1 });
	model.mesh()->add_element(fea::mesh::elements::type::beam2, { 1, 2 });
	
	//refine
	fea::mesh::cells::Line::refine(0, 5);
	fea::mesh::cells::Line::refine(1, 5);
	//supports
	model.boundary()->add_support(0, fea::mesh::nodes::dof::rotation_z);
	model.boundary()->add_support(2, fea::mesh::nodes::dof::rotation_z);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_y);
	model.boundary()->add_support(2, fea::mesh::nodes::dof::translation_x);

	//loads
	model.boundary()->add_load_case(2, fea::mesh::nodes::dof::translation_y, -5e5);

	//solver
	model.analysis()->solver(fea::analysis::solvers::type::static_nonlinear);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->step_max(200);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_max(4.0);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_predictor(0.1);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->watch_dof(2, fea::mesh::nodes::dof::translation_y);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->strategy(fea::analysis::strategies::type::arc_length_cylindric);

	//solve
	model.analysis()->solve();

	//save
	model.save();
}
