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
#include "Mesh/Cells/Line/Line.h"
#include "Mesh/Materials/Types.h"
#include "Mesh/Elements/Mechanic/Mechanic.h"
#include "Mesh/Elements/Mechanic/Frame/Bar.h"
#include "Mesh/Materials/Mechanic/Associative/Steel.h"

#include "Boundary/Boundary.h"
#include "Boundary/Cases/Load_Case.h"
#include "Boundary/Dependencies/Dependency.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Types.h"
#include "Analysis/Strategies/Types.h"
#include "Analysis/Solvers/Static_Nonlinear.h"

//ben
#include "benchmarks/mechanic/bar.h"

void tests::bar::static_linear::truss_carol(void)
{
	
	//parameters
	const unsigned n = 6;
	const double w = 4.00;
	const double h = 3.00;
	
	//model
	fea::models::Model model("truss carol", "benchmarks/bar/static/linear");

	//nodes
	for(unsigned i = 0; i <= n; i++)
	{
		model.mesh()->add_node(w * i, 0, 0);
	}
	for(unsigned i = 1; i < n; i++)
	{
		model.mesh()->add_node(w * i, h, 0);
	}

	//cells
	model.mesh()->add_cell(fea::mesh::cells::type::bar);

	//materials
	model.mesh()->add_material(fea::mesh::materials::type::steel);

	//sections
	model.mesh()->add_section(fea::mesh::sections::type::ring);
	((fea::mesh::sections::Ring*) model.mesh()->section(0))->diameter(0.02);
	((fea::mesh::sections::Ring*) model.mesh()->section(0))->thickness(0.002);

	//elements
	for(unsigned i = 0; i < n; i++)
	{
		model.mesh()->add_element(fea::mesh::elements::type::bar, {i, i + 1});
	}
	for(unsigned i = 0; i < n - 2; i++)
	{
		model.mesh()->add_element(fea::mesh::elements::type::bar, {i + n + 1, i + n + 2});
	}
	for(unsigned i = 0; i < n - 1; i++)
	{
		model.mesh()->add_element(fea::mesh::elements::type::bar, {i + 1, i + n + 1});
	}
	for(unsigned i = 0; i < n / 2; i++)
	{
		model.mesh()->add_element(fea::mesh::elements::type::bar, {i, i + n + 1});
	}
	for(unsigned i = n / 2 + 1; i <= n; i++)
	{
		model.mesh()->add_element(fea::mesh::elements::type::bar, {i, i + n - 1});
	}

	//supports
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_y);
	model.boundary()->add_support(n, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(n, fea::mesh::nodes::dof::translation_y);
	for(unsigned i = 0; i < 2 * n; i++)
	{
		model.boundary()->add_support(i, fea::mesh::nodes::dof::translation_z);
	}
	
	//loads
	model.boundary()->add_load_case();
	for(unsigned i = 1; i < n; i++)
	{
		model.boundary()->load_case(0)->add_load_node(i, fea::mesh::nodes::dof::translation_y, -1e3);
	}

	//solver
	model.analysis()->solver(fea::analysis::solvers::type::static_linear);
	model.analysis()->solver()->watch_dof(n / 2, fea::mesh::nodes::dof::translation_y);

	//solve
	model.analysis()->solve();

	//save
	model.save();
}
