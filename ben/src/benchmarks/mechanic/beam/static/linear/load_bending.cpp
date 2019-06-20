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

#include "Boundary/Boundary.h"
#include "Boundary/Loads/Types.h"
#include "Boundary/Cases/Load_Case.h"
#include "Boundary/Loads/Elements/Element.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Types.h"
#include "Analysis/Solvers/Static_Linear.h"

//ben
#include "benchmarks/mechanic/beam.h"

void tests::beam::static_linear::load_bending(void)
{
	//model
	fea::models::Model model("load bending", "benchmarks/beam/static/linear");

	//nodes
	model.mesh()->add_node(0, 0, 0);
	model.mesh()->add_node(1, 0, 0);

	//cells
	model.mesh()->add_cell(fea::mesh::cells::type::beam);
	
	//materials
	model.mesh()->add_material(fea::mesh::materials::type::steel);

	//sections
	model.mesh()->add_section(fea::mesh::sections::type::rectangle);

	//elements
	model.mesh()->add_element(fea::mesh::elements::type::beam2, {0, 1});

	//refine
	fea::mesh::cells::Line::refine(0, 10);
	
	//supports
	model.boundary()->add_support(0, fea::mesh::nodes::dof::rotation_z);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_y);

	//loads
	model.boundary()->add_load_case(fea::boundary::loads::type::bending, {}, 1.44e+09);
	for(unsigned i = 0; i < 10; i++)
	{
		model.boundary()->load_case(0)->load_element(0)->add_element(i);
	}

	//solver
	model.analysis()->solver(fea::analysis::solvers::type::static_linear);
	model.analysis()->solver()->watch_dof(1, fea::mesh::nodes::dof::rotation_z);

	//solve
	model.analysis()->solve();
	
	//save
	model.save();
}
