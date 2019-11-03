//std
#include <cmath>

//fea
#include "Model/Model.h"

#include "Mesh/Mesh.h"
#include "Mesh/Nodes/Dofs.h"
#include "Mesh/Cells/Types.h"
#include "Mesh/Sections/Ring.h"
#include "Mesh/Sections/Types.h"
#include "Mesh/Elements/Types.h"
#include "Mesh/Materials/Types.h"
#include "Mesh/Elements/Mechanic/Mechanic.h"
#include "Mesh/Materials/Mechanic/Associative/Steel.h"

#include "Boundary/Boundary.h"
#include "Boundary/Cases/Load_Case.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Types.h"
#include "Analysis/Solvers/Dynamic_Nonlinear.h"

//ben
#include "benchmarks/mechanic/bar.h"

void tests::bar::dynamic_nonlinear::single_pendulum_3D(void) 
{
	/*
		single pendulum 3D
		h = f' * sin(x)^2 = cte
		x'' + g / l sin(x) - h^2 * cos(x) / sin(x)^3 = 0
	 */

	//model
	fea::models::Model model("single pendulum 3D", "benchmarks/bar/dynamic/nonlinear");

	//parameters
	const double g = 9.81e+0;
	const double d = 1.00e-1;
	const double t = 1.00e-2;
	const double l = 1.00e+0;
	const double m = 1.00e+0;
	const double E = 2.00e+9;
	
	const double v0 = 0;
	const double f0 = 00 * M_PI / 180;
	const double x0 = 60 * M_PI / 180;
	
	const double uz = l * (1 - cos(x0));
	const double ux = l * sin(x0) * cos(f0);
	const double uy = l * sin(x0) * sin(f0);
	
	const double df = sqrt(g / l / cos(x0));
	
	const double vz = l * sin(x0) * v0;
	const double vx = l * (cos(x0) * cos(f0) * v0 - sin(x0) * sin(f0) * df);
	const double vy = l * (cos(x0) * sin(f0) * v0 + sin(x0) * cos(f0) * df);
	
	const unsigned n = 5;

	//nodes
	model.mesh()->add_node(+0, +0, +0);
	model.mesh()->add_node(+0, +0, -l);

	//cells
	model.mesh()->add_cell(fea::mesh::cells::type::bar);

	//materials
	model.mesh()->add_material(fea::mesh::materials::type::steel);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->specific_mass(0);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->elastic_modulus(E);

	//sections
	model.mesh()->add_section(fea::mesh::sections::type::ring);
	((fea::mesh::sections::Ring*) model.mesh()->section(0))->diameter(d);
	((fea::mesh::sections::Ring*) model.mesh()->section(0))->thickness(t);

	//elements
	fea::mesh::elements::Mechanic::geometric(true);
	model.mesh()->add_element(fea::mesh::elements::type::bar, {0, 1});
	
	//initials
	model.boundary()->add_initial(1, fea::mesh::nodes::dof::translation_x, ux, vx);
	model.boundary()->add_initial(1, fea::mesh::nodes::dof::translation_y, uy, vy);
	model.boundary()->add_initial(1, fea::mesh::nodes::dof::translation_z, uz, vz);

	//supports
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_y);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_z);
	model.boundary()->add_support(1, fea::mesh::nodes::dof::translation_x, 0, 0, m);
	model.boundary()->add_support(1, fea::mesh::nodes::dof::translation_y, 0, 0, m);
	model.boundary()->add_support(1, fea::mesh::nodes::dof::translation_z, 0, 0, m);

	//loads
	model.boundary()->add_load_case(1, fea::mesh::nodes::dof::translation_z, -m * g);

	//solver
	model.analysis()->solver(fea::analysis::solvers::type::dynamic_nonlinear);
	dynamic_cast<fea::analysis::solvers::Dynamic_Nonlinear*> (model.analysis()->solver())->step_max(800);
	dynamic_cast<fea::analysis::solvers::Dynamic_Nonlinear*> (model.analysis()->solver())->time_max(2 * n * M_PI * sqrt(l / g));
	dynamic_cast<fea::analysis::solvers::Dynamic_Nonlinear*> (model.analysis()->solver())->watch_dof(1, fea::mesh::nodes::dof::translation_z);
	dynamic_cast<fea::analysis::solvers::Dynamic_Nonlinear*> (model.analysis()->solver())->integration(fea::analysis::solvers::integration::newmark);

	//solve
	model.analysis()->solve();
	
	//save
	model.save();
}
