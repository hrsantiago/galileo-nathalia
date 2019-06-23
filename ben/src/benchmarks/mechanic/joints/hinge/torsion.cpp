//std
#include <cmath>

//fea
#include "Model/Model.h"

#include "Mesh/Mesh.h"
#include "Mesh/Nodes/Dofs.h"
#include "Mesh/Joints/Types.h"
#include "Mesh/Joints/Hinge.h"
#include "Mesh/Materials/Types.h"

#include "Boundary/Boundary.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Types.h"
#include "Analysis/Solvers/Static_Nonlinear.h"

//ben
#include "benchmarks/mechanic/joints.h"

void tests::joint::hinge::torsion(void)
{
	//model
	fea::models::Model model("torsion", "benchmarks/joints/hinge");

	//nodes
	model.mesh()->add_node(0, 0, 0);
	model.mesh()->add_node(0, 0, 0);

	//joints
	model.mesh()->add_joint(fea::mesh::joints::type::hinge, {0, 1});
	((fea::mesh::joints::Hinge*) model.mesh()->joint(0))->moment_yield(5.0e2);
	((fea::mesh::joints::Hinge*) model.mesh()->joint(0))->plastic_modulus(1.0e-2);
	((fea::mesh::joints::Hinge*) model.mesh()->joint(0))->stiffness(0, 1.0e5, 1.0e5, 1.0e3);

	//supports
	model.boundary()->add_support(0, fea::mesh::nodes::dof::rotation_x);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::rotation_y);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::rotation_z);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_y);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_z);

	//loads
	model.boundary()->add_load_case(1, fea::mesh::nodes::dof::rotation_z, 1.0);

	//solver
	model.analysis()->solver(fea::analysis::solvers::type::static_nonlinear);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->step_max(400);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_predictor(5.0);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->watch_dof(1, fea::mesh::nodes::dof::rotation_z);

	//solve
	model.analysis()->solve();
	
	//save
	model.save();
}
