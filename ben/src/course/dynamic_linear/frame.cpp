//std
#include <cmath>

//course
#include "course/course.h"

//fea
#include "Model/Model.h"

#include "Mesh/Mesh.h"
#include "Mesh/Nodes/Dofs.h"
#include "Mesh/Cells/Types.h"
#include "Mesh/Sections/Types.h"
#include "Mesh/Elements/Types.h"
#include "Mesh/Cells/Line/Line.h"
#include "Mesh/Materials/Types.h"
#include "Mesh/Sections/Rectangle.h"
#include "Mesh/Elements/Mechanic/Mechanic.h"
#include "Mesh/Materials/Mechanic/Associative/Steel.h"

#include "Boundary/Boundary.h"
#include "Boundary/Cases/Load_Case.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Types.h"
#include "Analysis/Solvers/Modal.h"

void course::dynamic_linear::frame(void)
{
	//parameters
	const unsigned n = 5;
	const unsigned m = 10;
	const double a = 2.00e-01;
	const double b = 2.00e-01;
	const double w = 4.00e+00;
	const double h = 3.00e+00;
	const double E = 2.00e+11;

	//model
	fea::models::Model model("frame", "course/dynamic_linear");

	//nodes
	for(unsigned i = 0; i <= n; i++)
	{
		for(unsigned j = 0; j <= m; j++)
		{
			model.mesh()->add_node(i * w, j * h, 0);
		}
	}

	//cells
	model.mesh()->add_cell(fea::mesh::cells::type::beam);

	//materials
	model.mesh()->add_material(fea::mesh::materials::type::steel);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->elastic_modulus(E);

	//sections
	model.mesh()->add_section(fea::mesh::sections::type::rectangle);
	((fea::mesh::sections::Rectangle*) model.mesh()->section(0))->width(a);
	((fea::mesh::sections::Rectangle*) model.mesh()->section(0))->height(b);

	//elements
	for(unsigned i = 0; i <= n; i++)
	{
		for(unsigned j = 0; j < m; j++)
		{
			model.mesh()->add_element(fea::mesh::elements::type::beam2, {(m + 1) * i + j, (m + 1) * i + j + 1});
		}
	}
	for(unsigned i = 1; i <= m; i++)
	{
		for(unsigned j = 0; j < n; j++)
		{
			model.mesh()->add_element(fea::mesh::elements::type::beam2, {(m + 1) * j + i, (m + 1) * (j + 1) + i});
		}
	}
	
	for(unsigned i = 0; i < (2 * n + 1) * m; i++)
	{
		fea::mesh::cells::Line::refine(i, 5);
	}

	//supports
	for(unsigned i = 0; i <= n; i++)
	{
		model.boundary()->add_support((m + 1) * i, fea::mesh::nodes::dof::rotation_z);
		model.boundary()->add_support((m + 1) * i, fea::mesh::nodes::dof::translation_x);
		model.boundary()->add_support((m + 1) * i, fea::mesh::nodes::dof::translation_y);
	}
	for(unsigned i = 1; i <= m; i++)
	{
		model.boundary()->add_support(n * (m + 1) + i, fea::mesh::nodes::dof::translation_x, 0, 1e4, 0);
	}
	
	model.boundary()->add_load_case();
	for(unsigned i = 1; i <= m; i++)
	{
		model.boundary()->load_case(0)->add_load_node(i, fea::mesh::nodes::dof::translation_x, 1e6, [] (double t) { return sin(12.57 * t); });
	}

	//solver
	model.analysis()->solver(fea::analysis::solvers::type::dynamic_linear);
	
	model.analysis()->solver()->time_max(10.0);
	model.analysis()->solver()->step_max(2000);
	model.analysis()->solver()->watch_dof(m, fea::mesh::nodes::dof::translation_x);

	//solve
	model.analysis()->solve();
	
	//save
	model.save();
}
