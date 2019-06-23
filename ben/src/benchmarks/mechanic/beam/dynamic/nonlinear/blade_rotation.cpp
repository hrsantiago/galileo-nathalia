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
#include "Mesh/Cells/Line/Line.h"
#include "Mesh/Materials/Types.h"
#include "Mesh/Sections/Generic.h"
#include "Mesh/Sections/Rectangle.h"
#include "Mesh/Elements/Mechanic/Mechanic.h"
#include "Mesh/Materials/Mechanic/Associative/Steel.h"

#include "Boundary/Boundary.h"
#include "Boundary/Supports/Support.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Types.h"
#include "Analysis/Solvers/Dynamic_Nonlinear.h"

//ben
#include "benchmarks/mechanic/beam.h"

void tests::beam::dynamic_nonlinear::blade_rotation(void) 
{
	/*
	Blade subjeted to rotation
	Literature:
	H. B. Coda and R. B. Paccola (2014) - Finite Elements in Analysis and Design - pp. 1 - 15
	 */

	//model
	fea::models::Model model("blade rotation", "benchmarks/beam/dynamic/nonlinear");

	//nodes
	model.mesh()->add_node(0e0, 0e0, 0e0);
	model.mesh()->add_node(1e1, 0e0, 0e0);

	//cells
	model.mesh()->add_cell(fea::mesh::cells::type::beam);

	//materials
	model.mesh()->add_material(fea::mesh::materials::type::steel);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->specific_mass(8.57e3);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->elastic_modulus(2.00e11);

	//sections
	model.mesh()->add_section(fea::mesh::sections::type::generic);
	((fea::mesh::sections::Generic*) model.mesh()->section(0))->area(1.40e-4);
	((fea::mesh::sections::Generic*) model.mesh()->section(0))->inercia_z(7.00e-8);

	//elements
	fea::mesh::elements::Mechanic::geometric(true);
	model.mesh()->add_element(fea::mesh::elements::type::beam2, {0, 1});

	//refine
	fea::mesh::cells::Line::refine(0, 10);

	//supports
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_y);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::rotation_z);

	//kinematics
	std::function<double(double) > u = [](double t) 
	{
		return t < 15 ? 2.0 / 5 * (pow(t, 2) / 2 + pow(15 / (2 * M_PI), 2) * (cos(2 * M_PI * t / 15) - 1)) : 6 * t - 45;
	};
	std::function<double(double) > v = [](double t) 
	{
		return t < 15 ? 2.0 / 5 * (t - 15 / (2 * M_PI) * sin(2 * M_PI * t / 15)) : 6.0;
	};
	std::function<double(double) > a = [](double t) 
	{
		return t < 15 ? 2.0 / 5 * (1 - cos(2 * M_PI * t / 15)) : 0.0;
	};
	model.boundary()->support(2)->state(u);
	model.boundary()->support(2)->velocity(v);
	model.boundary()->support(2)->acceleration(a);

	//loads
	model.boundary()->add_load_case();

	//solver
	model.analysis()->solver(fea::analysis::solvers::type::dynamic_nonlinear);
	dynamic_cast<fea::analysis::solvers::Dynamic_Nonlinear*> (model.analysis()->solver())->time_max(30.0);
	dynamic_cast<fea::analysis::solvers::Dynamic_Nonlinear*> (model.analysis()->solver())->step_max(5000);
	dynamic_cast<fea::analysis::solvers::Dynamic_Nonlinear*> (model.analysis()->solver())->tolerance(1e-2);
	dynamic_cast<fea::analysis::solvers::Dynamic_Nonlinear*> (model.analysis()->solver())->time_adjust(true);
	dynamic_cast<fea::analysis::solvers::Dynamic_Nonlinear*> (model.analysis()->solver())->iteration_desired(2);
	dynamic_cast<fea::analysis::solvers::Dynamic_Nonlinear*> (model.analysis()->solver())->watch_dof(1, fea::mesh::nodes::dof::rotation_z);
	dynamic_cast<fea::analysis::solvers::Dynamic_Nonlinear*> (model.analysis()->solver())->integration(fea::analysis::solvers::integration::newmark);

	//solve
	model.analysis()->solve();
	
	//save
	model.save();
}
