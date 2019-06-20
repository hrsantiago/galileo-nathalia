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

void tests::deployable::state::slut_grid(void)
{
	//model
	fea::models::Model model("slut grid", "benchmarks/deployable/state");
	
	//parameters
	const double l = 1;
	const double a = 0;
	const double b = M_PI / 2;
	
	const unsigned nx = 10;
	const unsigned ny = 10;
	
	//nodes
	for(unsigned i = 0; i < ny; i++)
	{
		for(unsigned j = 0; j < nx; j++)
		{
			model.mesh()->add_node((2 * j + 0) * l * sin(a), (2 * i + 0) * l * cos(a), 0);
			model.mesh()->add_node((2 * j + 1) * l * sin(a), (2 * i + 1) * l * cos(a), 0);
			model.mesh()->add_node((2 * j + 2) * l * sin(a), (2 * i + 2) * l * cos(a), 0);
			model.mesh()->add_node((2 * j + 2) * l * sin(a), (2 * i + 0) * l * cos(a), 0);
			model.mesh()->add_node((2 * j + 1) * l * sin(a), (2 * i + 1) * l * cos(a), 0);
			model.mesh()->add_node((2 * j + 0) * l * sin(a), (2 * i + 2) * l * cos(a), 0);
		}
	}
	
	//cells
	model.mesh()->add_cell(fea::mesh::cells::type::beam);

	//materials
	model.mesh()->add_material(fea::mesh::materials::type::steel);

	//sections
	model.mesh()->add_section(fea::mesh::sections::type::rectangle);
	
	//elements
	fea::mesh::elements::Mechanic::geometric(true);
	for(unsigned i = 0; i < ny; i++)
	{
		for(unsigned j = 0; j < nx; j++)
		{
			model.mesh()->add_element(fea::mesh::elements::type::beam2, {6 * (nx * i + j) + 0, 6 * (nx * i + j) + 1});
			model.mesh()->add_element(fea::mesh::elements::type::beam2, {6 * (nx * i + j) + 1, 6 * (nx * i + j) + 2});
			model.mesh()->add_element(fea::mesh::elements::type::beam2, {6 * (nx * i + j) + 3, 6 * (nx * i + j) + 4});
			model.mesh()->add_element(fea::mesh::elements::type::beam2, {6 * (nx * i + j) + 4, 6 * (nx * i + j) + 5});
		}
	}
	
	//joints
	for(unsigned i = 0; i < ny; i++)
	{
		for(unsigned j = 0; j < nx; j++)
		{
			model.mesh()->add_joint(fea::mesh::joints::type::pinned, {6 * (nx * i + j) + 4, 6 * (nx * i + j) + 1});
			if(j + 1 != nx)
			{
				model.mesh()->add_joint(fea::mesh::joints::type::pinned, {6 * (nx * i + j) + 3, 6 * (nx * i + j + 1) + 0});
			}
			else if(i + 1 != ny)
			{
				model.mesh()->add_joint(fea::mesh::joints::type::pinned, {6 * (nx * i + j) + 2, 6 * (nx * (i + 1) + j) + 3});
			}
			if(i + 1 != ny)
			{
				model.mesh()->add_joint(fea::mesh::joints::type::pinned, {6 * (nx * i + j) + 5, 6 * (nx * (i + 1) + j) + 0});
			}
			else if(j + 1 != nx)
			{
				model.mesh()->add_joint(fea::mesh::joints::type::pinned, {6 * (nx * i + j) + 2, 6 * (nx * i + j + 1) + 5});
			}
			if(j + 1 != nx && i + 1 != ny)
			{
				model.mesh()->add_joint(fea::mesh::joints::type::pinned, {6 * (nx * i + j) + 2, 6 * (nx * (i + 1) + j + 1) + 0});
			}
		}
	}
	
	//supports
	for(unsigned i = 0; i < ny; i++)
	{
		for(unsigned j = 0; j < nx; j++)
		{
			std::function<double(double)> r1 = [i, j, l, a] (double t) { return -t; };
			std::function<double(double)> r2 = [i, j, l, a] (double t) { return +t; };
			std::function<double(double)> u0 = [i, j, l, a] (double t) { return (2 * j + 0) * l * (sin(a + t) - sin(a)); };
			std::function<double(double)> u1 = [i, j, l, a] (double t) { return (2 * j + 1) * l * (sin(a + t) - sin(a)); };
			std::function<double(double)> u2 = [i, j, l, a] (double t) { return (2 * j + 2) * l * (sin(a + t) - sin(a)); };
			std::function<double(double)> v0 = [i, j, l, a] (double t) { return (2 * i + 0) * l * (cos(a + t) - cos(a)); };
			std::function<double(double)> v1 = [i, j, l, a] (double t) { return (2 * i + 1) * l * (cos(a + t) - cos(a)); };
			std::function<double(double)> v2 = [i, j, l, a] (double t) { return (2 * i + 2) * l * (cos(a + t) - cos(a)); };
			model.boundary()->add_support(6 * (nx * i + j) + 0, fea::mesh::nodes::dof::rotation_z, r1);
			model.boundary()->add_support(6 * (nx * i + j) + 1, fea::mesh::nodes::dof::rotation_z, r1);
			model.boundary()->add_support(6 * (nx * i + j) + 2, fea::mesh::nodes::dof::rotation_z, r1);
			model.boundary()->add_support(6 * (nx * i + j) + 3, fea::mesh::nodes::dof::rotation_z, r2);
			model.boundary()->add_support(6 * (nx * i + j) + 4, fea::mesh::nodes::dof::rotation_z, r2);
			model.boundary()->add_support(6 * (nx * i + j) + 5, fea::mesh::nodes::dof::rotation_z, r2);
			model.boundary()->add_support(6 * (nx * i + j) + 0, fea::mesh::nodes::dof::translation_x, u0);
			model.boundary()->add_support(6 * (nx * i + j) + 0, fea::mesh::nodes::dof::translation_y, v0);
			model.boundary()->add_support(6 * (nx * i + j) + 1, fea::mesh::nodes::dof::translation_x, u1);
			model.boundary()->add_support(6 * (nx * i + j) + 1, fea::mesh::nodes::dof::translation_y, v1);
			model.boundary()->add_support(6 * (nx * i + j) + 2, fea::mesh::nodes::dof::translation_x, u2);
			model.boundary()->add_support(6 * (nx * i + j) + 2, fea::mesh::nodes::dof::translation_y, v2);
			model.boundary()->add_support(6 * (nx * i + j) + 3, fea::mesh::nodes::dof::translation_x, u2);
			model.boundary()->add_support(6 * (nx * i + j) + 3, fea::mesh::nodes::dof::translation_y, v0);
			model.boundary()->add_support(6 * (nx * i + j) + 4, fea::mesh::nodes::dof::translation_x, u1);
			model.boundary()->add_support(6 * (nx * i + j) + 4, fea::mesh::nodes::dof::translation_y, v1);
			model.boundary()->add_support(6 * (nx * i + j) + 5, fea::mesh::nodes::dof::translation_x, u0);
			model.boundary()->add_support(6 * (nx * i + j) + 5, fea::mesh::nodes::dof::translation_y, v2);
		}
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
