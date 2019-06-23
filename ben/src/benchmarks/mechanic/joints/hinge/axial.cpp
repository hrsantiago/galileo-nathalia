//std
#include <cmath>

//fea
#include "Model/Model.h"

#include "Mesh/Mesh.h"
#include "Mesh/Nodes/Dofs.h"
#include "Mesh/Joints/Types.h"

#include "Boundary/Boundary.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Types.h"
#include "Analysis/Solvers/Static_Nonlinear.h"

//ben
#include "benchmarks/mechanic/joints.h"

void tests::joint::hinge::axial(void)
{
	//model
	fea::models::Model model("axial", "benchmarks/joints/hinge");

	//nodes
	model.mesh()->add_node(0, 0, 0);
	model.mesh()->add_node(0, 0, 0);

	//joints
	model.mesh()->add_joint(fea::mesh::joints::type::hinge, {0, 1});

	//supports
	model.boundary()->add_support(0, fea::mesh::nodes::dof::rotation_x);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::rotation_y);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::rotation_z);
	model.boundary()->add_support(1, fea::mesh::nodes::dof::rotation_z);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_y);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_z);

	//loads
	model.boundary()->add_load_case(1, fea::mesh::nodes::dof::translation_z, 1e10);

	//solver
	model.analysis()->solver(fea::analysis::solvers::type::static_nonlinear);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->dof_max(1e-2);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->step_max(100);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_predictor(1e-4);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->watch_dof(1, fea::mesh::nodes::dof::translation_z);

	//solve
	model.analysis()->solve();
	
	//save
	model.save();
}
