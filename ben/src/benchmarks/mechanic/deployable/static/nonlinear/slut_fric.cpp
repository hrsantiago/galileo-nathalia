//std
#include <cmath>

//mat
#include "misc/defs.h"

//fea
#include "Models/Model.h"

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

#include "Boundary/Boundary.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Types.h"
#include "Analysis/Strategies/Types.h"
#include "Analysis/Solvers/Static_Nonlinear.h"

//ben
#include "benchmarks/mechanic/deployable.h"

void tests::deployable::static_nonlinear::slut_fric(void)
{
	//model
	fea::models::Model model("slut fric", "benchmarks/deployable/static/nonlinear");

	//parameters
	const double l = 1.00e+00;
	const double h = 4.00e-02;
	const double t = 1.00e-02;
	const double s = 1.00e-02;
	const double a = M_PI / 2;
	const double f = 0;//(t + s) / 2;
	
	const double my = 1.00e+00;
	const double kt = 1.00e+05;
	const double kp = 1.00e-03;

	//nodes
	model.mesh()->add_node(0 * l * cos(a), 0 * l * sin(a), -f);
	model.mesh()->add_node(1 * l * cos(a), 1 * l * sin(a), -f);
	model.mesh()->add_node(2 * l * cos(a), 2 * l * sin(a), -f);
	model.mesh()->add_node(2 * l * cos(a), 0 * l * sin(a), +f);
	model.mesh()->add_node(1 * l * cos(a), 1 * l * sin(a), +f);
	model.mesh()->add_node(0 * l * cos(a), 2 * l * sin(a), +f);
	
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
	model.mesh()->add_element(fea::mesh::elements::type::beam3, {0, 1});
	model.mesh()->add_element(fea::mesh::elements::type::beam3, {1, 2});
	model.mesh()->add_element(fea::mesh::elements::type::beam3, {3, 4});
	model.mesh()->add_element(fea::mesh::elements::type::beam3, {4, 5});

	//joints
	model.mesh()->add_joint(fea::mesh::joints::type::hinge, {1, 4});
	((fea::mesh::joints::Hinge*) model.mesh()->joint(0))->moment_yield(my);
	((fea::mesh::joints::Hinge*) model.mesh()->joint(0))->plastic_modulus(kp);
	((fea::mesh::joints::Hinge*) model.mesh()->joint(0))->stiffness(0, 2, kt);

	//supports
	model.boundary()->add_support(0, fea::mesh::nodes::dof::rotation_x);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::rotation_y);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_y);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_z);
	model.boundary()->add_support(3, fea::mesh::nodes::dof::translation_y);
	model.boundary()->add_support(5, fea::mesh::nodes::dof::translation_x);

	//loads
	model.boundary()->add_load_case(3, fea::mesh::nodes::dof::translation_x, my / l);

	//solver
	model.analysis()->solver(fea::analysis::solvers::type::static_nonlinear);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_max(1e1);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->step_max(900);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_adjust(true);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->dof_min(-M_PI / 2);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_predictor(1e+1);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->iteration_desired(3);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->watch_dof(0, fea::mesh::nodes::dof::rotation_z);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->strategy(fea::analysis::strategies::type::minimal_norm);

	//solve
	model.analysis()->solve();
	
	//save
	model.save();
}
