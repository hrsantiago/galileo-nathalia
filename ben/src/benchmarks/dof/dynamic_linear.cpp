//std
#include <cmath>
#include <functional>

//fea
#include "Model/Model.h"

#include "Mesh/Mesh.h"
#include "Mesh/Nodes/Dofs.h"

#include "Boundary/Boundary.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Types.h"
#include "Analysis/Solvers/Dynamic_Linear.h"

//ben
#include "benchmarks/dof/dof.h"

void tests::dof::dynamic_linear(void) 
{
	//model
	fea::models::Model model("linear", "benchmarks/dof/dynamic");

	//parameters
	const double m = 1.0;
	const double c = 0.1;
	const double k = 1.0;
	const double u = 1.0;
	const double v = 1.0;
	const double f = 1.0;
	const double w = 1.0;

	//nodes
	model.mesh()->add_node(0, 0, 0);

	//initials
	model.boundary()->add_initial(0, fea::mesh::nodes::dof::translation_x, u, v);

	//supports
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_x, k, c, m);

	//loads
	model.boundary()->add_load_case(0, fea::mesh::nodes::dof::translation_x, f, [w] (double t) { return sin(w * t); });

	//solver
	model.analysis()->solver(fea::analysis::solvers::type::dynamic_linear);
	dynamic_cast<fea::analysis::solvers::Dynamic_Linear*> (model.analysis()->solver())->step_max(700);
	dynamic_cast<fea::analysis::solvers::Dynamic_Linear*> (model.analysis()->solver())->time_max(1e2);

	//solve
	model.analysis()->solve();
	
	//save
	model.save();
}
