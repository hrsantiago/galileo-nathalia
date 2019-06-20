//std
#include <cmath>

//mat
#include "misc/defs.h"

//fea
#include "Models/Model.h"

#include "Mesh/Mesh.h"
#include "Mesh/Nodes/Dofs.h"
#include "Mesh/Cells/Types.h"
#include "Mesh/Sections/Types.h"
#include "Mesh/Elements/Types.h"
#include "Mesh/Materials/Types.h"
#include "Mesh/Sections/Rectangle.h"
#include "Mesh/Elements/Mechanic/Mechanic.h"
#include "Mesh/Materials/Mechanic/Associative/Steel.h"

#include "Boundary/Boundary.h"
#include "Boundary/Cases/Load_Case.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Types.h"
#include "Analysis/Strategies/Types.h"
#include "Analysis/Solvers/Static_Nonlinear.h"

//ben
#include "benchmarks/mechanic/bar.h"

void tests::bar::static_nonlinear::dome_star(void) 
{
	/*
	Dome subjeted to vertical load
	Literature: Thesis UFOP L. Pinheiro (2003) p. 150
	 */

	//model
	fea::models::Model model("dome star", "benchmarks/bar/static/nonlinear");

	//parameters
	const double d = 1.000e-1;
	const double E = 1.000e+6;
	const double r = 2.500e+1;
	const double R = 4.330e+1;
	const double h = 6.216e+0;
	const double H = 8.216e+0;
	
	const double A = d * d;

	//nodes
	model.mesh()->add_node(0, 0, H);
	for(unsigned i = 0; i < 6; i++) 
	{
		const double t = 2 * M_PI * i / 6;
		model.mesh()->add_node(r * cos(t), r * sin(t), h);
	}
	for(unsigned i = 0; i < 6; i++) 
	{
		const double t = 2 * M_PI * (i + 0.5) / 6;
		model.mesh()->add_node(R * cos(t), R * sin(t), 0);
	}

	//materials
	model.mesh()->add_material(fea::mesh::materials::type::steel);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->elastic_modulus(E);

	//sections
	model.mesh()->add_section(fea::mesh::sections::type::rectangle);
	((fea::mesh::sections::Rectangle*) model.mesh()->section(0))->width(d);
	((fea::mesh::sections::Rectangle*) model.mesh()->section(0))->height(d);

	//cells
	model.mesh()->add_cell(fea::mesh::cells::type::bar);

	//elements
	fea::mesh::elements::Mechanic::geometric(true);
	for(unsigned i = 0; i < 6; i++) 
	{
		model.mesh()->add_element(fea::mesh::elements::type::bar, {0, i + 1});
		model.mesh()->add_element(fea::mesh::elements::type::bar, {i + 1, i != 5 ? i + 2 : 1});
		model.mesh()->add_element(fea::mesh::elements::type::bar, {i + 1, i + 7});
		model.mesh()->add_element(fea::mesh::elements::type::bar, {i + 1, i != 0 ? i + 6 : 12});
	}

	//dependencies
//	model.boundary()->add_dependency(2, 1, fea::mesh::nodes::dof::translation_z, fea::mesh::nodes::dof::translation_z);
//	model.boundary()->add_dependency(3, 1, fea::mesh::nodes::dof::translation_z, fea::mesh::nodes::dof::translation_z);
//	model.boundary()->add_dependency(4, 1, fea::mesh::nodes::dof::translation_z, fea::mesh::nodes::dof::translation_z);
//	model.boundary()->add_dependency(5, 1, fea::mesh::nodes::dof::translation_z, fea::mesh::nodes::dof::translation_z);
//	model.boundary()->add_dependency(6, 1, fea::mesh::nodes::dof::translation_z, fea::mesh::nodes::dof::translation_z);

//	model.boundary()->add_dependency(1, 1, fea::mesh::nodes::dof::translation_y, fea::mesh::nodes::dof::translation_x, 0, sin(  0 * M_PI / 180));
//	model.boundary()->add_dependency(2, 1, fea::mesh::nodes::dof::translation_x, fea::mesh::nodes::dof::translation_x, 0, cos( 60 * M_PI / 180));
//	model.boundary()->add_dependency(2, 1, fea::mesh::nodes::dof::translation_y, fea::mesh::nodes::dof::translation_x, 0, sin( 60 * M_PI / 180));
//	model.boundary()->add_dependency(3, 1, fea::mesh::nodes::dof::translation_x, fea::mesh::nodes::dof::translation_x, 0, cos(120 * M_PI / 180));
//	model.boundary()->add_dependency(3, 1, fea::mesh::nodes::dof::translation_y, fea::mesh::nodes::dof::translation_x, 0, sin(120 * M_PI / 180));
//	model.boundary()->add_dependency(4, 1, fea::mesh::nodes::dof::translation_x, fea::mesh::nodes::dof::translation_x, 0, cos(180 * M_PI / 180));
//	model.boundary()->add_dependency(4, 1, fea::mesh::nodes::dof::translation_y, fea::mesh::nodes::dof::translation_x, 0, sin(180 * M_PI / 180));
//	model.boundary()->add_dependency(5, 1, fea::mesh::nodes::dof::translation_x, fea::mesh::nodes::dof::translation_x, 0, cos(240 * M_PI / 180));
//	model.boundary()->add_dependency(5, 1, fea::mesh::nodes::dof::translation_y, fea::mesh::nodes::dof::translation_x, 0, sin(240 * M_PI / 180));
//	model.boundary()->add_dependency(6, 1, fea::mesh::nodes::dof::translation_x, fea::mesh::nodes::dof::translation_x, 0, cos(300 * M_PI / 180));
//	model.boundary()->add_dependency(6, 1, fea::mesh::nodes::dof::translation_y, fea::mesh::nodes::dof::translation_x, 0, sin(300 * M_PI / 180));

	//supports
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_y);
	for(unsigned i = 7; i < 13; i++) 
	{
		model.boundary()->add_support(i, fea::mesh::nodes::dof::translation_x);
		model.boundary()->add_support(i, fea::mesh::nodes::dof::translation_y);
		model.boundary()->add_support(i, fea::mesh::nodes::dof::translation_z);
	}

	//loads
	model.boundary()->add_load_case(0, fea::mesh::nodes::dof::translation_z, -1);

	//solver
	model.analysis()->solver(fea::analysis::solvers::type::static_nonlinear);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->step_max(160);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_max(3.5);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_predictor(1.2);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_increment_max(1.0);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->watch_dof(0, fea::mesh::nodes::dof::translation_z);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->strategy(fea::analysis::strategies::type::arc_length_cylindric);

	//solve
	model.analysis()->solve();
	
	//save
	model.save();
}
