//std
#include <cmath>

//fea
#include "Model/Model.h"

#include "Mesh/Mesh.h"
#include "Mesh/Nodes/Dofs.h"
#include "Mesh/Cells/Types.h"
#include "Mesh/Joints/Types.h"
#include "Mesh/Sections/Types.h"
#include "Mesh/Elements/Types.h"
#include "Mesh/Materials/Types.h"
#include "Mesh/Elements/Mechanic/Mechanic.h"

#include "Boundary/Boundary.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Types.h"
#include "Analysis/Solvers/State.h"

//ben
#include "benchmarks/mechanic/deployable.h"

//state
void tests::deployable::state::slut_line(void)
{
	//model
	fea::models::Model model("slut line", "benchmarks/deployable/state");

	//parameters
	const double l = 1;
	const double a = 0;
	const double b = M_PI / 2;

	//nodes
	model.mesh()->add_node(0 * l * sin(a), 0 * l * cos(a), 0);
	model.mesh()->add_node(1 * l * sin(a), 1 * l * cos(a), 0);
	model.mesh()->add_node(2 * l * sin(a), 2 * l * cos(a), 0);
	model.mesh()->add_node(2 * l * sin(a), 0 * l * cos(a), 0);
	model.mesh()->add_node(1 * l * sin(a), 1 * l * cos(a), 0);
	model.mesh()->add_node(0 * l * sin(a), 2 * l * cos(a), 0);

	//cells
	model.mesh()->add_cell(fea::mesh::cells::type::beam);

	//materials
	model.mesh()->add_material(fea::mesh::materials::type::steel);

	//sections
	model.mesh()->add_section(fea::mesh::sections::type::rectangle);

	//elements
	fea::mesh::elements::Mechanic::geometric(true);
	model.mesh()->add_element(fea::mesh::elements::type::beam2, {0, 1});
	model.mesh()->add_element(fea::mesh::elements::type::beam2, {1, 2});
	model.mesh()->add_element(fea::mesh::elements::type::beam2, {3, 4});
	model.mesh()->add_element(fea::mesh::elements::type::beam2, {4, 5});

	//joints
	model.mesh()->add_joint(fea::mesh::joints::type::pinned, {1, 4});

	//kinematics
	std::function<double(double)> r1 = [l, a] (double t) { return -t; };
	std::function<double(double)> r2 = [l, a] (double t) { return +t; };
	std::function<double(double)> u1 = [l, a] (double t) { return 1 * l * (sin(a + t) - sin(a)); };
	std::function<double(double)> u2 = [l, a] (double t) { return 2 * l * (sin(a + t) - sin(a)); };
	std::function<double(double)> v1 = [l, a] (double t) { return 1 * l * (cos(a + t) - cos(a)); };
	std::function<double(double)> v2 = [l, a] (double t) { return 2 * l * (cos(a + t) - cos(a)); };

	//supports
	model.boundary()->add_support(0, fea::mesh::nodes::dof::rotation_z, r1);
	model.boundary()->add_support(1, fea::mesh::nodes::dof::rotation_z, r1);
	model.boundary()->add_support(2, fea::mesh::nodes::dof::rotation_z, r1);
	model.boundary()->add_support(3, fea::mesh::nodes::dof::rotation_z, r2);
	model.boundary()->add_support(4, fea::mesh::nodes::dof::rotation_z, r2);
	model.boundary()->add_support(5, fea::mesh::nodes::dof::rotation_z, r2);
	model.boundary()->add_support(1, fea::mesh::nodes::dof::translation_x, u1);
	model.boundary()->add_support(1, fea::mesh::nodes::dof::translation_y, v1);
	model.boundary()->add_support(2, fea::mesh::nodes::dof::translation_x, u2);
	model.boundary()->add_support(2, fea::mesh::nodes::dof::translation_y, v2);
	model.boundary()->add_support(3, fea::mesh::nodes::dof::translation_x, u2);
	model.boundary()->add_support(4, fea::mesh::nodes::dof::translation_x, u1);
	model.boundary()->add_support(4, fea::mesh::nodes::dof::translation_y, v1);
	model.boundary()->add_support(5, fea::mesh::nodes::dof::translation_y, v2);

	//solver
	model.analysis()->solver(fea::analysis::solvers::type::state);
	dynamic_cast<fea::analysis::solvers::State*> (model.analysis()->solver())->time_max(b - a);
	dynamic_cast<fea::analysis::solvers::State*> (model.analysis()->solver())->watch_dof(0, fea::mesh::nodes::dof::rotation_z);

	//solve
	model.analysis()->solve();
	
	//save
	model.save();
}
