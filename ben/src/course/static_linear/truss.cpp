//course
#include "course/course.h"

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
#include "Mesh/Materials/Mechanic/Associative/Steel.h"

#include "Boundary/Boundary.h"
#include "Boundary/Cases/Load_Case.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Types.h"
#include "Analysis/Solvers/Modal.h"

void course::static_linear::truss(void)
{
	//parameters
	const unsigned n = 5;
	const double d = 1.00e-01;
	const double t = 1.00e-02;
	const double w = 4.00e+00;
	const double h = 3.00e+00;
	const double E = 2.00e+11;

	//model
	fea::models::Model model("truss", "course/static_linear");

	//nodes
	for(unsigned i = 0; i <= n; i++)
	{
		model.mesh()->add_node(i * w, 0, 0);
	}
	for(unsigned i = 0; i < n; i++)
	{
		model.mesh()->add_node(i * w + w / 2, h, 0);
	}

	//cells
	model.mesh()->add_cell(fea::mesh::cells::type::bar);

	//materials
	model.mesh()->add_material(fea::mesh::materials::type::steel);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->elastic_modulus(E);

	//sections
	model.mesh()->add_section(fea::mesh::sections::type::ring);
	((fea::mesh::sections::Ring*) model.mesh()->section(0))->diameter(d);
	((fea::mesh::sections::Ring*) model.mesh()->section(0))->thickness(t);

	//elements
	for(unsigned i = 0; i < n; i++)
	{
		model.mesh()->add_element(fea::mesh::elements::type::bar, {i, i + 1});
	}
	for(unsigned i = 0; i < n - 1; i++)
	{
		model.mesh()->add_element(fea::mesh::elements::type::bar, {i + n + 1, i + n + 2});
	}
	for(unsigned i = 0; i < n; i++)
	{
		model.mesh()->add_element(fea::mesh::elements::type::bar, {i, i + n + 1});
	}
	for(unsigned i = 0; i < n; i++)
	{
		model.mesh()->add_element(fea::mesh::elements::type::bar, {i + 1, i + n + 1});
	}

	//supports
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_y);
	model.boundary()->add_support(n, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(n, fea::mesh::nodes::dof::translation_y);
	for(unsigned i = 0; i < 2 * n + 1; i++)
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
