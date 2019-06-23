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

void tests::bar::static_nonlinear::dome_circle(void) 
{
	/*
	Dome subjeted to vertical load
	Literature: Thesis UFOP L. Pinheiro (2003) p. 161
	 */

	//model
	fea::models::Model model("dome circle", "benchmarks/bar/static/nonlinear");

	//parameters
	const unsigned n = 24;
	const double r1 = 1e-2 * 110.99;
	const double r2 = 1e-2 * 203.30;
	const double r3 = 1e-2 * 290.00;
	const double h1 = 1e-2 * 34.083;
	const double h2 = 1e-2 * 156.947;
	const double h3 = 1e-2 * 179.022;
	const double E = 200e9;
	const double d = 0.1;
	const double A = d * d;

	//nodes
	model.mesh()->add_node(0, 0, h3);
	for(unsigned i = 0; i < n; i++) 
	{
		const double t = 2 * M_PI * i / n;
		model.mesh()->add_node(r1 * cos(t), r1 * sin(t), h2);
	}
	for(unsigned i = 0; i < n; i++)
	 {
		const double t = 2 * M_PI * (i + 0.5) / n;
		model.mesh()->add_node(r2 * cos(t), r2 * sin(t), h1);
	}
	for(unsigned i = 0; i < 24; i++) 
	{
		const double t = 2 * M_PI * i / n;
		model.mesh()->add_node(r3 * cos(t), r3 * sin(t), 0);
	}

	//cells
	model.mesh()->add_cell(fea::mesh::cells::type::bar);

	//materials
	model.mesh()->add_material(fea::mesh::materials::type::steel);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->elastic_modulus(E);

	//sections
	model.mesh()->add_section(fea::mesh::sections::type::rectangle);
	((fea::mesh::sections::Rectangle*) model.mesh()->section(0))->width(d);
	((fea::mesh::sections::Rectangle*) model.mesh()->section(0))->height(d);

	//elements
	fea::mesh::elements::Mechanic::geometric(true);
	for(unsigned i = 0; i < 24; i++) 
	{
		model.mesh()->add_element(fea::mesh::elements::type::bar, {0, i + 1});
		model.mesh()->add_element(fea::mesh::elements::type::bar, {i + 1, i + n + 1});
		model.mesh()->add_element(fea::mesh::elements::type::bar, {i + n + 1, i + 2 * n + 1});
		model.mesh()->add_element(fea::mesh::elements::type::bar, {i + 1, i != n - 1 ? i + 2 : 1});
		model.mesh()->add_element(fea::mesh::elements::type::bar, {i + 1, i != 0 ? i + n : 2 * n});
		model.mesh()->add_element(fea::mesh::elements::type::bar, {i + n + 1, i != n - 1 ? i + n + 2 : n + 1});
		model.mesh()->add_element(fea::mesh::elements::type::bar, {i + n + 1, i != n - 1 ? i + 2 * n + 2 : 2 * n + 1});
	}

	//dependencies
	const double dt = M_PI / n;
//	model.boundary()->add_dependency(1, 1, fea::mesh::nodes::dof::translation_y, fea::mesh::nodes::dof::translation_x, 0, 0);
//	model.boundary()->add_dependency(n + 1, n + 1, fea::mesh::nodes::dof::translation_y, fea::mesh::nodes::dof::translation_x, 0, sin(dt) / cos(dt));
	for(unsigned i = 2; i < n + 1; i++) 
	{
		const double t1 = 2 * (i - 1.0) * dt;
		const double t2 = 2 * (i - 0.5) * dt;
//		model.boundary()->add_dependency(i, 1, fea::mesh::nodes::dof::translation_z, fea::mesh::nodes::dof::translation_z);
//		model.boundary()->add_dependency(i, 1, fea::mesh::nodes::dof::translation_x, fea::mesh::nodes::dof::translation_x, 0, cos(t1));
//		model.boundary()->add_dependency(i, 1, fea::mesh::nodes::dof::translation_y, fea::mesh::nodes::dof::translation_x, 0, sin(t1));
//		model.boundary()->add_dependency(i + 24, n + 1, fea::mesh::nodes::dof::translation_z, fea::mesh::nodes::dof::translation_z);
//		model.boundary()->add_dependency(i + 24, n + 1, fea::mesh::nodes::dof::translation_x, fea::mesh::nodes::dof::translation_x, 0, cos(t2) / cos(dt));
//		model.boundary()->add_dependency(i + 24, n + 1, fea::mesh::nodes::dof::translation_y, fea::mesh::nodes::dof::translation_x, 0, sin(t2) / cos(dt));
	}

	//supports
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_y);
	for(unsigned i = 0; i < n; i++) 
	{
		model.boundary()->add_support(i + 2 * n + 1, fea::mesh::nodes::dof::translation_x);
		model.boundary()->add_support(i + 2 * n + 1, fea::mesh::nodes::dof::translation_y);
		model.boundary()->add_support(i + 2 * n + 1, fea::mesh::nodes::dof::translation_z);
	}

	//loads
	model.boundary()->add_load_case(0, fea::mesh::nodes::dof::translation_z, -1e-1 * E * A);

	//solver
	model.analysis()->solver(fea::analysis::solvers::type::static_nonlinear);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->step_max(130);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_max(0.2);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_predictor(0.01);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_increment_max(0.1);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->strategy(fea::analysis::strategies::type::arc_length_cylindric);

	//solve
	model.analysis()->solve();
	
	//save
	model.save();
}
