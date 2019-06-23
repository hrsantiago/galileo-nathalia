//std
#include <cmath>

//mat
#include "misc/defs.h"

//fea
#include "Model/Model.h"

#include "Mesh/Mesh.h"
#include "Mesh/Nodes/Dofs.h"
#include "Mesh/Cells/Types.h"
#include "Mesh/Sections/Types.h"
#include "Mesh/Elements/Types.h"
#include "Mesh/Materials/Types.h"
#include "Mesh/Sections/Circle.h"
#include "Mesh/Elements/Mechanic/Mechanic.h"
#include "Mesh/Materials/Mechanic/Associative/Steel.h"

#include "Boundary/Boundary.h"
#include "Boundary/Cases/Load_Case.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Types.h"
#include "Analysis/Solvers/Dynamic_Nonlinear.h"

//ben
#include "benchmarks/mechanic/bar.h"

void tests::bar::dynamic_nonlinear::double_pendulum_2D(void)
{
	//model
	fea::models::Model model("double pendulum 2D", "benchmarks/bar/dynamic/nonlinear");

	//parameters
	const double g = 9.81e+0;
	const double d = 1.00e-2;
	const double E = 2.00e+11;
	
	const double m1 = 1.00e+0;
	const double m2 = 1.00e+0;
	const double l1 = 1.00e+0;
	const double l2 = 1.00e+0;
	
	const double lt = l1 + l2;
	const double t1 =  90 * M_PI / 180;
	const double t2 = 180 * M_PI / 180;
	
	const double u1 = l1 * sin(t1);
	const double v1 = l1 * (1 - cos(t1));
	const double u2 = l1 * sin(t1) + l2 * sin(t2);
	const double v2 = l1 * (1 - cos(t1)) + l2 * (1 - cos(t2));
	
	const unsigned n = 20;

	//nodes
	model.mesh()->add_node(0,   0, 0);
	model.mesh()->add_node(0, -l1, 0);
	model.mesh()->add_node(0, -lt, 0);

	//cells
	model.mesh()->add_cell(fea::mesh::cells::type::bar);

	//materials
	model.mesh()->add_material(fea::mesh::materials::type::steel);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->specific_mass(0);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->elastic_modulus(E);

	//sections
	model.mesh()->add_section(fea::mesh::sections::type::circle);
	((fea::mesh::sections::Circle*) model.mesh()->section(0))->diameter(d);

	//elements
	fea::mesh::elements::Mechanic::geometric(true);
	model.mesh()->add_element(fea::mesh::elements::type::bar, {0, 1});
	model.mesh()->add_element(fea::mesh::elements::type::bar, {1, 2});

	//initials
	model.boundary()->add_initial(1, fea::mesh::nodes::dof::translation_x, u1, 0);
	model.boundary()->add_initial(1, fea::mesh::nodes::dof::translation_y, v1, 0);
	model.boundary()->add_initial(2, fea::mesh::nodes::dof::translation_x, u2, 0);
	model.boundary()->add_initial(2, fea::mesh::nodes::dof::translation_y, v2, 0);

	//supports
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_y);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_z);
	model.boundary()->add_support(1, fea::mesh::nodes::dof::translation_z);
	model.boundary()->add_support(2, fea::mesh::nodes::dof::translation_z);
	model.boundary()->add_support(1, fea::mesh::nodes::dof::translation_x, 0, 0, m1);
	model.boundary()->add_support(1, fea::mesh::nodes::dof::translation_y, 0, 0, m1);
	model.boundary()->add_support(2, fea::mesh::nodes::dof::translation_x, 0, 0, m2);
	model.boundary()->add_support(2, fea::mesh::nodes::dof::translation_y, 0, 0, m2);

	//loads
	model.boundary()->add_load_case();
	model.boundary()->load_case(0)->add_load_node(1, fea::mesh::nodes::dof::translation_y, -m1 * g);
	model.boundary()->load_case(0)->add_load_node(2, fea::mesh::nodes::dof::translation_y, -m2 * g);

	//solver
	model.analysis()->solver(fea::analysis::solvers::type::dynamic_nonlinear);
	dynamic_cast<fea::analysis::solvers::Dynamic_Nonlinear*> (model.analysis()->solver())->step_max(10000);
	dynamic_cast<fea::analysis::solvers::Dynamic_Nonlinear*> (model.analysis()->solver())->time_max(2 * M_PI * n * sqrt(lt / g));
	dynamic_cast<fea::analysis::solvers::Dynamic_Nonlinear*> (model.analysis()->solver())->watch_dof(2, fea::mesh::nodes::dof::translation_y);
	dynamic_cast<fea::analysis::solvers::Dynamic_Nonlinear*> (model.analysis()->solver())->integration(fea::analysis::solvers::integration::newmark);

	//solve
	model.analysis()->solve();
	
	//save
	model.save();
}
