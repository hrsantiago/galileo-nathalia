//std
#include <cmath>
#include <functional>

//mat
#include "misc/defs.h"

//fea
#include "Model/Model.h"

#include "Mesh/Mesh.h"
#include "Mesh/Nodes/Dofs.h"

#include "Boundary/Boundary.h"
#include "Boundary/Dependencies/Dependency.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Types.h"

//ben
#include "benchmarks/dof/dof.h"

void tests::dof::dependencies(void)
{
	//model
	fea::models::Model model("dependencies", "benchmarks/dof");

	//nodes
	model.mesh()->add_node(0, 0, 0);

	//supports
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_x, [] (double t) { return cos(2 * M_PI * t); });

	//dependencies
	model.boundary()->add_dependency(0, fea::mesh::nodes::dof::translation_y, { 0 }, { fea::mesh::nodes::dof::translation_x });
	model.boundary()->dependency(0)->state([] (double* d) { return d[0] * d[0]; });

	//loads
	model.boundary()->add_load_case();

	//solver
	model.analysis()->solver(fea::analysis::solvers::type::state);

	//solve
	model.analysis()->solve();
	
	//save
	model.save();
}
