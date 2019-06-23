//fea
#include "Model/Model.h"

#include "Mesh/Mesh.h"
#include "Mesh/Nodes/Dofs.h"

#include "Boundary/Boundary.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Types.h"

//ben
#include "benchmarks/dof/dof.h"

void tests::dof::modal(void) 
{
	//model
	fea::models::Model model("modal", "benchmarks/dof");

	//nodes
	model.mesh()->add_node(0, 0, 0);

	//supports
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_x, 1, 0, 4);

	//loads
	model.boundary()->add_load_case();

	//solver
	model.analysis()->solver(fea::analysis::solvers::type::modal);

	//solve
	model.analysis()->solve();
	
	//save
	model.save();
}
