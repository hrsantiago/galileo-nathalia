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

void tests::deployable::state::slut_unit(void)
{
	//model
	fea::models::Model model("slut unit", "benchmarks/deployable/state");
	
	//parameters
	const double l = 1;
	const double a = 0;
	const double b = M_PI / 2;
	
	const unsigned n = 10;
	
	//nodes
	for(unsigned i = 0; i < n; i++)
	{
		model.mesh()->add_node((2 * i + 0) * l * sin(a), 0 * l * cos(a), 0);
		model.mesh()->add_node((2 * i + 1) * l * sin(a), 1 * l * cos(a), 0);
		model.mesh()->add_node((2 * i + 2) * l * sin(a), 2 * l * cos(a), 0);
		model.mesh()->add_node((2 * i + 2) * l * sin(a), 0 * l * cos(a), 0);
		model.mesh()->add_node((2 * i + 1) * l * sin(a), 1 * l * cos(a), 0);
		model.mesh()->add_node((2 * i + 0) * l * sin(a), 2 * l * cos(a), 0);
	}
	
	//cells
	model.mesh()->add_cell(fea::mesh::cells::type::beam);

	//materials
	model.mesh()->add_material(fea::mesh::materials::type::steel);

	//sections
	model.mesh()->add_section(fea::mesh::sections::type::rectangle);
	
	//elements
	fea::mesh::elements::Mechanic::geometric(true);
	for(unsigned i = 0; i < n; i++)
	{
		model.mesh()->add_element(fea::mesh::elements::type::beam2, {6 * i + 0, 6 * i + 1});
		model.mesh()->add_element(fea::mesh::elements::type::beam2, {6 * i + 1, 6 * i + 2});
		model.mesh()->add_element(fea::mesh::elements::type::beam2, {6 * i + 3, 6 * i + 4});
		model.mesh()->add_element(fea::mesh::elements::type::beam2, {6 * i + 4, 6 * i + 5});
	}
	
	//joints
	for(unsigned i = 0; i < n; i++)
	{
		model.mesh()->add_joint(fea::mesh::joints::type::pinned, {6 * i + 4, 6 * i + 1});
		if(i + 1 != n)
		{
			model.mesh()->add_joint(fea::mesh::joints::type::pinned, {6 * i + 3, 6 * (i + 1) + 0});
			model.mesh()->add_joint(fea::mesh::joints::type::pinned, {6 * i + 2, 6 * (i + 1) + 5});
		}
	}
	
	//supports
	for(unsigned i = 0; i < n; i++)
	{
		std::function<double(double)> r1 = [i, l, a] (double t) { return -t; };
		std::function<double(double)> r2 = [i, l, a] (double t) { return +t; };
		std::function<double(double)> v1 = [i, l, a] (double t) { return 1 * l * (cos(a + t) - cos(a)); };
		std::function<double(double)> v2 = [i, l, a] (double t) { return 2 * l * (cos(a + t) - cos(a)); };
		std::function<double(double)> u0 = [i, l, a] (double t) { return (2 * i + 0) * l * (sin(a + t) - sin(a)); };
		std::function<double(double)> u1 = [i, l, a] (double t) { return (2 * i + 1) * l * (sin(a + t) - sin(a)); };
		std::function<double(double)> u2 = [i, l, a] (double t) { return (2 * i + 2) * l * (sin(a + t) - sin(a)); };
		model.boundary()->add_support(6 * i + 0, fea::mesh::nodes::dof::rotation_z, r1);
		model.boundary()->add_support(6 * i + 1, fea::mesh::nodes::dof::rotation_z, r1);
		model.boundary()->add_support(6 * i + 2, fea::mesh::nodes::dof::rotation_z, r1);
		model.boundary()->add_support(6 * i + 3, fea::mesh::nodes::dof::rotation_z, r2);
		model.boundary()->add_support(6 * i + 4, fea::mesh::nodes::dof::rotation_z, r2);
		model.boundary()->add_support(6 * i + 5, fea::mesh::nodes::dof::rotation_z, r2);
		model.boundary()->add_support(6 * i + 0, fea::mesh::nodes::dof::translation_x, u0);
		model.boundary()->add_support(6 * i + 1, fea::mesh::nodes::dof::translation_x, u1);
		model.boundary()->add_support(6 * i + 1, fea::mesh::nodes::dof::translation_y, v1);
		model.boundary()->add_support(6 * i + 2, fea::mesh::nodes::dof::translation_x, u2);
		model.boundary()->add_support(6 * i + 2, fea::mesh::nodes::dof::translation_y, v2);
		model.boundary()->add_support(6 * i + 3, fea::mesh::nodes::dof::translation_x, u2);
		model.boundary()->add_support(6 * i + 4, fea::mesh::nodes::dof::translation_x, u1);
		model.boundary()->add_support(6 * i + 4, fea::mesh::nodes::dof::translation_y, v1);
		model.boundary()->add_support(6 * i + 5, fea::mesh::nodes::dof::translation_x, u0);
		model.boundary()->add_support(6 * i + 5, fea::mesh::nodes::dof::translation_y, v2);
	}
	
	//solver
	model.analysis()->solver(fea::analysis::solvers::type::state);
	dynamic_cast<fea::analysis::solvers::State*> (model.analysis()->solver())->time_max(b - a);
	dynamic_cast<fea::analysis::solvers::State*> (model.analysis()->solver())->watch_dof(0, fea::mesh::nodes::dof::rotation_z);

	//solve
	model.analysis()->solve();
	
	//save
	model.save();
}
