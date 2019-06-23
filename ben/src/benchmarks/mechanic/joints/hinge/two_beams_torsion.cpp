//std
#include <cmath>

//mat
#include "misc/defs.h"

//fea
#include "Model/Model.h"

#include "Mesh/Mesh.h"
#include "Mesh/Nodes/Dofs.h"
#include "Mesh/Cells/Types.h"
#include "Mesh/Joints/Types.h"
#include "Mesh/Joints/Hinge.h"
#include "Mesh/Sections/Types.h"
#include "Mesh/Elements/Types.h"
#include "Mesh/Materials/Types.h"
#include "Mesh/Sections/Rectangle.h"
#include "Mesh/Elements/Mechanic/Mechanic.h"
#include "Mesh/Elements/Mechanic/Frame/Beam3.h"

#include "Boundary/Boundary.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Types.h"
#include "Analysis/Strategies/Types.h"
#include "Analysis/Solvers/Static_Nonlinear.h"

//ben
#include "benchmarks/mechanic/joints.h"

void tests::joint::hinge::two_beams_torsion(void)
{
	//model
	fea::models::Model model("two beams torsion", "benchmarks/joints/hinge");

	//nodes
	model.mesh()->add_node(0, 0, 0);
	model.mesh()->add_node(1, 0, 0);
	model.mesh()->add_node(1, 0, 0);
	model.mesh()->add_node(2, 0, 0);

	//celss
	model.mesh()->add_cell(fea::mesh::cells::type::beam);

	//materials
	model.mesh()->add_material(fea::mesh::materials::type::steel);

	//sections
	model.mesh()->add_section(fea::mesh::sections::type::rectangle);
	((fea::mesh::sections::Rectangle*) model.mesh()->section(0))->width(1e-2);
	((fea::mesh::sections::Rectangle*) model.mesh()->section(0))->height(4e-2);

	//elements
	fea::mesh::elements::Mechanic::geometric(true);
	model.mesh()->add_element(fea::mesh::elements::type::beam3, {0, 1});
	model.mesh()->add_element(fea::mesh::elements::type::beam3, {2, 3});

	//joints
	model.mesh()->add_joint(fea::mesh::joints::type::hinge, {1, 2});
	((fea::mesh::joints::Hinge*) model.mesh()->joint(0))->stiffness(0, 1.0e0, 1.0e0, 1.0e0);

	//supports
	model.boundary()->add_support(0, fea::mesh::nodes::dof::rotation_x);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::rotation_y);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::rotation_z);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_y);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_z);
	
	model.boundary()->add_support(1, fea::mesh::nodes::dof::rotation_y);
	model.boundary()->add_support(1, fea::mesh::nodes::dof::rotation_z);
	model.boundary()->add_support(1, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(1, fea::mesh::nodes::dof::translation_y);
	model.boundary()->add_support(1, fea::mesh::nodes::dof::translation_z);
	
	model.boundary()->add_support(2, fea::mesh::nodes::dof::rotation_y);
	model.boundary()->add_support(2, fea::mesh::nodes::dof::rotation_z);
	model.boundary()->add_support(2, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(2, fea::mesh::nodes::dof::translation_y);
	model.boundary()->add_support(2, fea::mesh::nodes::dof::translation_z);
	
	model.boundary()->add_support(3, fea::mesh::nodes::dof::rotation_y);
	model.boundary()->add_support(3, fea::mesh::nodes::dof::rotation_z);
	model.boundary()->add_support(3, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(3, fea::mesh::nodes::dof::translation_y);
	model.boundary()->add_support(3, fea::mesh::nodes::dof::translation_z);

	//dependencies
//	model.boundary()->add_dependency(1, fea::mesh::nodes::dof::rotation_x, { 0 }, { fea::mesh::nodes::dof::rotation_x });
//	model.boundary()->add_dependency(2, fea::mesh::nodes::dof::rotation_x, { 3 }, { fea::mesh::nodes::dof::rotation_x });

	//loads
	model.boundary()->add_load_case(3, fea::mesh::nodes::dof::rotation_x, 1.0e0);

	//solver
	model.analysis()->solver(fea::analysis::solvers::type::static_nonlinear);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_max(1e0);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->step_max(2000);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->dof_max(M_PI / 18);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_predictor(1e-3);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->watch_dof(3, fea::mesh::nodes::dof::rotation_x);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->strategy(fea::analysis::strategies::type::control_load);

	//solve
	model.analysis()->solve();
	
	//save
	model.save();
}
