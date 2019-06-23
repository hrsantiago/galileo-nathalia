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
#include "Mesh/Joints/Pinned.h"
#include "Mesh/Sections/Types.h"
#include "Mesh/Elements/Types.h"
#include "Mesh/Materials/Types.h"
#include "Mesh/Sections/Rectangle.h"
#include "Mesh/Elements/Mechanic/Mechanic.h"

#include "Boundary/Boundary.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Types.h"
#include "Analysis/Strategies/Types.h"
#include "Analysis/Solvers/Static_Nonlinear.h"

//ben
#include "benchmarks/mechanic/joints.h"

void tests::joint::pinned::scissor(void)
{
	//model
	fea::models::Model model("scissor", "benchmarks/joints/pinned");

	//parameters
	const double l = 1.00e+00;
	const double h = 4.00e-01;
	const double t = 1.00e-01;
	const double k = 1.00e+03;
	const double a = M_PI / 2;

	//nodes
	model.mesh()->add_node(0 * l * cos(a), 0 * l * sin(a), 0);
	model.mesh()->add_node(1 * l * cos(a), 1 * l * sin(a), 0);
	model.mesh()->add_node(2 * l * cos(a), 2 * l * sin(a), 0);
	model.mesh()->add_node(2 * l * cos(a), 0 * l * sin(a), 0);
	model.mesh()->add_node(1 * l * cos(a), 1 * l * sin(a), 0);
	model.mesh()->add_node(0 * l * cos(a), 2 * l * sin(a), 0);
	
	//cells
	model.mesh()->add_cell(fea::mesh::cells::type::beam);

	//materials
	model.mesh()->add_material(fea::mesh::materials::type::steel);

	//sections
	model.mesh()->add_section(fea::mesh::sections::type::rectangle);
	((fea::mesh::sections::Rectangle*) model.mesh()->section(0))->width(t);
	((fea::mesh::sections::Rectangle*) model.mesh()->section(0))->height(h);

	//elements
	fea::mesh::elements::Mechanic::geometric(true);
	model.mesh()->add_element(fea::mesh::elements::type::beam2, {0, 1});
	model.mesh()->add_element(fea::mesh::elements::type::beam2, {1, 2});
	model.mesh()->add_element(fea::mesh::elements::type::beam2, {3, 4});
	model.mesh()->add_element(fea::mesh::elements::type::beam2, {4, 5});

	//joints
	model.mesh()->add_joint(fea::mesh::joints::type::pinned, {1, 4});
	((fea::mesh::joints::Pinned*) model.mesh()->joint(0))->stiffness(k);

	//supports
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_y);
	model.boundary()->add_support(3, fea::mesh::nodes::dof::translation_y);
	model.boundary()->add_support(5, fea::mesh::nodes::dof::translation_x);

	//loads
	model.boundary()->add_load_case(3, fea::mesh::nodes::dof::translation_x, k / l);

	//solver
	model.analysis()->solver(fea::analysis::solvers::type::static_nonlinear);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_max(1e2);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->step_max(900);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_adjust(false);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->dof_min(-M_PI / 2);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_predictor(0.01);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->iteration_desired(3);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->watch_dof(0, fea::mesh::nodes::dof::rotation_z);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->strategy(fea::analysis::strategies::type::minimal_norm);

	//solve
	model.analysis()->solve();
	
	//save
	model.save();
}
