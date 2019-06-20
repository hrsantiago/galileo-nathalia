//std
#include <cmath>

//mat
#include "misc/defs.h"

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

void tests::beam::static_nonlinear::elastic::frame_roof(void) 
{
	/*
	Elastic roof frame (primary path)
	Literature: International Journal of nonlinear mechanics - N. Xiao, H. Zhong (2012) pp. 633-640
	 */

	//model
	fea::models::Model model("frame roof", "benchmarks/beam/static/nonlinear/elastic");

	//parameters
	const double l = 60;
	const double t = M_PI / 6;
	
	const double c = cos(t);
	const double s = sin(t);

	//nodes
	model.mesh()->add_node(0, 0, 0);
	model.mesh()->add_node(0, l, 0);
	model.mesh()->add_node(l * c, l * (1 + s), 0);
	model.mesh()->add_node(2 * l * c, l, 0);
	model.mesh()->add_node(2 * l * c, 0, 0);

	//cells
	model.mesh()->add_cell(fea::mesh::cells::type::beam);
	
	//materials
	model.mesh()->add_material(fea::mesh::materials::type::steel);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->elastic_modulus(720);

	//sections
	model.mesh()->add_section(fea::mesh::sections::type::rectangle);
	((fea::mesh::sections::Rectangle*) model.mesh()->section(0))->width(3);
	((fea::mesh::sections::Rectangle*) model.mesh()->section(0))->height(1);

	//elements
	fea::mesh::elements::Mechanic::geometric(true);
	model.mesh()->add_element(fea::mesh::elements::type::beam3, {0, 1});
	model.mesh()->add_element(fea::mesh::elements::type::beam3, {1, 2});
	model.mesh()->add_element(fea::mesh::elements::type::beam3, {2, 3});
	model.mesh()->add_element(fea::mesh::elements::type::beam3, {3, 4});

	//refine
	fea::mesh::cells::Line::refine(0, 10);
	fea::mesh::cells::Line::refine(1, 10);
	fea::mesh::cells::Line::refine(2, 10);
	fea::mesh::cells::Line::refine(3, 10);

	//supports
	model.boundary()->add_support(0, fea::mesh::nodes::dof::rotation_x);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::rotation_y);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::rotation_z);
	model.boundary()->add_support(4, fea::mesh::nodes::dof::rotation_x);
	model.boundary()->add_support(4, fea::mesh::nodes::dof::rotation_y);
	model.boundary()->add_support(4, fea::mesh::nodes::dof::rotation_z);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_y);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_z);
	model.boundary()->add_support(4, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(4, fea::mesh::nodes::dof::translation_y);
	model.boundary()->add_support(4, fea::mesh::nodes::dof::translation_z);

	//loads
	model.boundary()->add_load_case(fea::boundary::loads::type::shear, {1, 2}, -8.33e-4);
	for(unsigned i = 0; i < 18; i++)
	{
		model.boundary()->load_case(0)->load_element(0)->add_element(13 + i);
	}

	//solver
	model.analysis()->solver(fea::analysis::solvers::type::static_nonlinear);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->step_max(200);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_max(10.0);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_predictor(1.00);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->strategy(fea::analysis::strategies::type::arc_length_cylindric);

	//solve
	model.analysis()->solve();
	
	//save
	model.save();
}
