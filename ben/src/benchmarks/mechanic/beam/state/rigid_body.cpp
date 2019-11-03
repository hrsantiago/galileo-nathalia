//std
#include <cmath>
#include <ctime>

//mat
#include "linear/dense.h"

//fea
#include "Model/Model.h"

#include "Mesh/Mesh.h"
#include "Mesh/Nodes/Dofs.h"
#include "Mesh/Cells/Types.h"
#include "Mesh/Sections/Types.h"
#include "Mesh/Elements/Types.h"
#include "Mesh/Materials/Types.h"
#include "Mesh/Elements/Mechanic/Mechanic.h"

#include "Boundary/Boundary.h"
#include "Boundary/Supports/Support.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Types.h"
#include "Analysis/Solvers/State.h"

//ben
#include "benchmarks/mechanic/beam.h"

void tests::beam::state::rigid_body(void) 
{
	//model
	fea::models::Model model("rigid body", "benchmarks/beam/state");

	//nodes
	model.mesh()->add_node(0, 0, 0);
	model.mesh()->add_node(1, 0, 0);

	//cells
	model.mesh()->add_cell(fea::mesh::cells::type::beam);

	//materials
	model.mesh()->add_material(fea::mesh::materials::type::steel);

	//sections
	model.mesh()->add_section(fea::mesh::sections::type::rectangle);

	//elements
	fea::mesh::elements::Mechanic::geometric(true);
	model.mesh()->add_element(fea::mesh::elements::type::beam3, {0, 1});

	//supports
	model.boundary()->add_support(0, fea::mesh::nodes::dof::rotation_x);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::rotation_y);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::rotation_z);
	model.boundary()->add_support(1, fea::mesh::nodes::dof::rotation_x);
	model.boundary()->add_support(1, fea::mesh::nodes::dof::rotation_y);
	model.boundary()->add_support(1, fea::mesh::nodes::dof::rotation_z);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_y);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_z);
	model.boundary()->add_support(1, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(1, fea::mesh::nodes::dof::translation_y);
	model.boundary()->add_support(1, fea::mesh::nodes::dof::translation_z);

	//kinematics
	double u[3], n[3];
	srand(time(nullptr));
	mat::randu(u, 3, 1);
	mat::normalize(mat::randu(n, 3, 1), 3);
	const std::function<double(double)> f[] = {
		[n] (double t) { return n[0] * t; },
		[n] (double t) { return n[1] * t; },
		[n] (double t) { return n[2] * t; },
		[n] (double t) { return n[0] * t; },
		[n] (double t) { return n[1] * t; },
		[n] (double t) { return n[2] * t; },
		[u] (double t) { return u[0] * t; },
		[u] (double t) { return u[1] * t; },
		[u] (double t) { return u[2] * t; },
		[u, n] (double t) { return u[0] * t + cos(t) + (1 - cos(t)) * n[0] * n[0] - 1; },
		[u, n] (double t) { return u[1] * t + n[2] * sin(t) + (1 - cos(t)) * n[1] * n[0]; },
		[u, n] (double t) { return u[2] * t - n[1] * sin(t) + (1 - cos(t)) * n[2] * n[0]; }
	};
	for(unsigned i = 0; i < 12; i++)
	{
		model.boundary()->support(i)->state(f[i]);
	}

	//solver
	model.analysis()->solver(fea::analysis::solvers::type::state);

	//solve
	model.analysis()->solve();
	
	//save
	model.save();
}
