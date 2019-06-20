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

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Types.h"
#include "Analysis/Strategies/Types.h"
#include "Analysis/Solvers/Static_Nonlinear.h"

//ben
#include "benchmarks/mechanic/beam.h"

void tests::beam::static_nonlinear::elastic::frame_williams(void) 
{
	/*
	Elastic Williams frame
	Literature: S. Krenk - Nonlinear modeling and analysis of solids and models (2009) pp. 116
	 */

	//model
	fea::models::Model model("frame williams", "benchmarks/beam/static/nonlinear/elastic");

	//parameters
	const double b = 1.00e+00;
	const double E = 2.00e+11;
	
	const double h = 0.024 * b;
	const double L = sqrt(b * b + h * h);
	const double d = 2e-2 * L;
	const double P = E * pow(d, 4) / (12 * L * L);

	//nodes
	model.mesh()->add_node(+0, h, 0);
	model.mesh()->add_node(-b, 0, 0);
	model.mesh()->add_node(+b, 0, 0);

	//cells
	model.mesh()->add_cell(fea::mesh::cells::type::beam);
	
	//materials
	model.mesh()->add_material(fea::mesh::materials::type::steel);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->elastic_modulus(E);

	//sections
	model.mesh()->add_section(fea::mesh::sections::type::rectangle);
	((fea::mesh::sections::Rectangle*) model.mesh()->section(0))->width(d);
	((fea::mesh::sections::Rectangle*) model.mesh()->section(0))->height(d);

	//elements
	fea::mesh::elements::Mechanic::geometric(true);
	model.mesh()->add_element(fea::mesh::elements::type::beam3, {1, 0});
	model.mesh()->add_element(fea::mesh::elements::type::beam3, {2, 0});

	//refine
	fea::mesh::cells::Line::refine(0, 4);
	fea::mesh::cells::Line::refine(1, 4);

	//supports
	model.boundary()->add_support(1, fea::mesh::nodes::dof::rotation_x);
	model.boundary()->add_support(1, fea::mesh::nodes::dof::rotation_y);
	model.boundary()->add_support(1, fea::mesh::nodes::dof::rotation_z);
	model.boundary()->add_support(2, fea::mesh::nodes::dof::rotation_z);
	model.boundary()->add_support(2, fea::mesh::nodes::dof::rotation_z);
	model.boundary()->add_support(2, fea::mesh::nodes::dof::rotation_z);
	model.boundary()->add_support(1, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(1, fea::mesh::nodes::dof::translation_y);
	model.boundary()->add_support(1, fea::mesh::nodes::dof::translation_z);
	model.boundary()->add_support(2, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(2, fea::mesh::nodes::dof::translation_y);
	model.boundary()->add_support(2, fea::mesh::nodes::dof::translation_z);

	//loads
	model.boundary()->add_load_case(0, fea::mesh::nodes::dof::translation_y, -P);

	//solver
	model.analysis()->solver(fea::analysis::solvers::type::static_nonlinear);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->step_max(200);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_max(1.5);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_predictor(0.1);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->strategy(fea::analysis::strategies::type::arc_length_cylindric);

	//solve
	model.analysis()->solve();
	
	//save
	model.save();
}
