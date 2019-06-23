//fea
#include "Model/Model.h"

#include "Mesh/Mesh.h"
#include "Mesh/Nodes/Dofs.h"

#include "Boundary/Boundary.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Types.h"

//ben
#include "benchmarks/dof/dof.h"

void tests::dof::static_linear(void)
{
	//model
	fea::models::Model model("linear", "benchmarks/dof/static");

	//nodes
	model.mesh()->add_node(0, 0, 0);

	//supports
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_x, 1, 0, 0);

	//loads
	model.boundary()->add_load_case(0, fea::mesh::nodes::dof::translation_x, 1);

	//solver
	model.analysis()->solver(fea::analysis::solvers::type::static_linear);

	//solve
	model.analysis()->solve();
	
	//save
	model.save();
}
