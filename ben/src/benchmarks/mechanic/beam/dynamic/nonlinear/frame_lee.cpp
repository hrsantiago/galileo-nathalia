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
#include "Analysis/Solvers/Dynamic_Nonlinear.h"

//ben
#include "benchmarks/mechanic/beam.h"

void tests::beam::dynamic_nonlinear::frame_lee(void)
{
	/*
	Lee frame
	Literature: T. N. Le et al. (2011) - Computer Mechanics - pp. 153 - 161
	 */

	//model
	fea::models::Model model("frame lee", "benchmarks/beam/dynamic/nonlinear");

	//nodes
	model.mesh()->add_node( 0,  0, 0);
	model.mesh()->add_node( 0, 12, 0);
	model.mesh()->add_node(12, 12, 0);

	//cells
	model.mesh()->add_cell(fea::mesh::cells::type::beam);
	
	//materials
	model.mesh()->add_material(fea::mesh::materials::type::steel);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->specific_mass(7.85e3);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->elastic_modulus(2.10e11);

	//sections
	model.mesh()->add_section(fea::mesh::sections::type::rectangle);
	((fea::mesh::sections::Rectangle*) model.mesh()->section(0))->width(0.30);
	((fea::mesh::sections::Rectangle*) model.mesh()->section(0))->height(0.20);

	//elements
	fea::mesh::elements::Mechanic::geometric(true);
	model.mesh()->add_element(fea::mesh::elements::type::beam2, {0, 1});
	model.mesh()->add_element(fea::mesh::elements::type::beam2, {1, 2});

	//refine
	fea::mesh::cells::Line::refine(0, 20);
	fea::mesh::cells::Line::refine(1, 20);

	//supports
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_y);
	model.boundary()->add_support(2, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(2, fea::mesh::nodes::dof::translation_y);

	//loads
	model.boundary()->add_load_case(25, fea::mesh::nodes::dof::translation_y, -4.1e6);

	//solver
	model.analysis()->solver(fea::analysis::solvers::type::dynamic_nonlinear);
	dynamic_cast<fea::analysis::solvers::Dynamic_Nonlinear*> (model.analysis()->solver())->time_max(4.00);
	dynamic_cast<fea::analysis::solvers::Dynamic_Nonlinear*> (model.analysis()->solver())->step_max(8000);
	dynamic_cast<fea::analysis::solvers::Dynamic_Nonlinear*> (model.analysis()->solver())->time_adjust(true);
	dynamic_cast<fea::analysis::solvers::Dynamic_Nonlinear*> (model.analysis()->solver())->iteration_desired(2);

	//solve
	model.analysis()->solve();
	
	//save
	model.save();
}
