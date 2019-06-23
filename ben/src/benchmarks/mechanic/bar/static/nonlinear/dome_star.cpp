//std
#include <cmath>

//mat
#include "misc/defs.h"

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
#include "Analysis/Strategies/Types.h"
#include "Analysis/Solvers/Static_Nonlinear.h"

//ben
#include "benchmarks/mechanic/bar.h"

void tests::bar::static_nonlinear::dome_star(void) 
{
	//model
	fea::models::Model model("dome star", "benchmarks/bar/static/nonlinear");

	//parameters
	const unsigned n = 6;
	const double d = 1.00e-1;
	const double t = 1.00e-2;
	const double E = 2.00e11;
	const double r = 2.00e+0;
	const double R = 4.00e+0;
	const double h = 2.00e+0;
	const double H = 3.00e+0;

	//nodes
	model.mesh()->add_node(0, 0, H);
	for(unsigned i = 0; i < n; i++) 
	{
		const double t = 2 * M_PI * i / n;
		model.mesh()->add_node(r * cos(t), r * sin(t), h);
	}
	for(unsigned i = 0; i < n; i++) 
	{
		const double t = 2 * M_PI * i / n + M_PI / n;
		model.mesh()->add_node(R * cos(t), R * sin(t), 0);
	}

	//cells
	model.mesh()->add_cell(fea::mesh::cells::type::bar);

	//sections
	model.mesh()->add_section(fea::mesh::sections::type::ring);
	((fea::mesh::sections::Ring*) model.mesh()->section(0))->diameter(d);
	((fea::mesh::sections::Ring*) model.mesh()->section(0))->thickness(t);

	//materials
	model.mesh()->add_material(fea::mesh::materials::type::steel);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->elastic_modulus(E);

	//elements
	for(unsigned i = 0; i < n; i++) 
	{
		model.mesh()->add_element(fea::mesh::elements::type::bar, {0, i + 1});
		model.mesh()->add_element(fea::mesh::elements::type::bar, {i + 1, i + n + 1});
		model.mesh()->add_element(fea::mesh::elements::type::bar, {i + 1, i + 1 != n ? i + 2 : 1});
		model.mesh()->add_element(fea::mesh::elements::type::bar, {i + 1, i != 0 ? i + n : 2 * n});
	}

	//dependencies
	for(unsigned i = 0; i < n; i++)
	{
		const double t = 2 * M_PI * i / n;
		model.boundary()->add_dependency(i + 1, fea::mesh::nodes::dof::translation_z, 1, fea::mesh::nodes::dof::translation_z);
		model.boundary()->add_dependency(i + 1, fea::mesh::nodes::dof::translation_x, 1, fea::mesh::nodes::dof::translation_x, cos(t));
		model.boundary()->add_dependency(i + 1, fea::mesh::nodes::dof::translation_y, 1, fea::mesh::nodes::dof::translation_x, sin(t));
	}
	model.boundary()->remove_dependency(0);
	model.boundary()->remove_dependency(0);

	//supports
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_y);
	for(unsigned i = n + 1; i < 2 * n + 1; i++) 
	{
		model.boundary()->add_support(i, fea::mesh::nodes::dof::translation_x);
		model.boundary()->add_support(i, fea::mesh::nodes::dof::translation_y);
		model.boundary()->add_support(i, fea::mesh::nodes::dof::translation_z);
	}

	//loads
	model.boundary()->add_load_case(0, fea::mesh::nodes::dof::translation_z, -1e6);

	//solver
	fea::mesh::elements::Mechanic::geometric(true);
	model.analysis()->solver(fea::analysis::solvers::type::static_nonlinear);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_max(1e3);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->step_max(2000);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_predictor(1.50);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->watch_dof(0, fea::mesh::nodes::dof::translation_z);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->strategy(fea::analysis::strategies::type::arc_length_cylindric);

	//solve
	model.analysis()->solve();
	
	//save
	model.save();
}
