//std
#include <cmath>

//fea
#include "Model/Model.h"

#include "Mesh/Mesh.h"
#include "Mesh/Nodes/Dofs.h"
#include "Mesh/Cells/Types.h"
#include "Mesh/Sections/Types.h"
#include "Mesh/Elements/Types.h"
#include "Mesh/Cells/Line/Line.h"
#include "Mesh/Sections/Circle.h"
#include "Mesh/Materials/Types.h"
#include "Mesh/Elements/Mechanic/Frame/Bar.h"
#include "Mesh/Materials/Mechanic/Associative/Steel.h"

#include "Boundary/Boundary.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Types.h"
#include "Analysis/Solvers/Buckling.h"

//ben
#include "benchmarks/mechanic/cable.h"

void tests::cable::buckling::mixed(void)
{
	//parameters
	const unsigned n = 50;
	const double b = 4.00e+00;
	const double H = 2.00e+01;
	const double l = 2.00e+01;
	const double h = 6.00e+00;
	const double E = 2.10e+11;
	const double A0 = 5.00e-05;
	const double A1 = 3.50e-03;
	const double A2 = 7.50e-04;
	const double d0 = sqrt(4 * A0 / M_PI);
	const double d1 = sqrt(4 * A1 / M_PI);
	const double d2 = sqrt(4 * A2 / M_PI);

	//model
	fea::models::Model model("mixed", "benchmarks/cable/buckling");

	//nodes
	model.mesh()->add_node(0, 0, H);
	model.mesh()->add_node(-b / 2, -b / 2, 0);
	model.mesh()->add_node(+b / 2, -b / 2, 0);
	model.mesh()->add_node(+b / 2, +b / 2, 0);
	model.mesh()->add_node(-b / 2, +b / 2, 0);
	model.mesh()->add_node(-b / 4, -b / 4, H / 2);
	model.mesh()->add_node(+b / 4, -b / 4, H / 2);
	model.mesh()->add_node(+b / 4, +b / 4, H / 2);
	model.mesh()->add_node(-b / 4, +b / 4, H / 2);
	
	model.mesh()->add_node(l, 0, H);
	model.mesh()->add_node(l - b / 2, -b / 2, 0);
	model.mesh()->add_node(l + b / 2, -b / 2, 0);
	model.mesh()->add_node(l + b / 2, +b / 2, 0);
	model.mesh()->add_node(l - b / 2, +b / 2, 0);
	model.mesh()->add_node(l - b / 4, -b / 4, H / 2);
	model.mesh()->add_node(l + b / 4, -b / 4, H / 2);
	model.mesh()->add_node(l + b / 4, +b / 4, H / 2);
	model.mesh()->add_node(l - b / 4, +b / 4, H / 2);
	
	for(unsigned i = 0; i <= n; i++)
	{
		const double t = double(i) / n;
		model.mesh()->add_node(l * t, 0, H - 4 * h * t * (1 - t));
	}

	//cells
	model.mesh()->add_cell(fea::mesh::cells::type::bar);
	model.mesh()->add_cell(fea::mesh::cells::type::bar);
	model.mesh()->add_cell(fea::mesh::cells::type::bar);
	((fea::mesh::cells::Line*) model.mesh()->cell(0))->section(0);
	((fea::mesh::cells::Line*) model.mesh()->cell(1))->section(1);
	((fea::mesh::cells::Line*) model.mesh()->cell(2))->section(2);

	//materials
	model.mesh()->add_material(fea::mesh::materials::type::steel);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->elastic_modulus(E);

	//sections
	model.mesh()->add_section(fea::mesh::sections::type::circle);
	model.mesh()->add_section(fea::mesh::sections::type::circle);
	model.mesh()->add_section(fea::mesh::sections::type::circle);
	((fea::mesh::sections::Circle*) model.mesh()->section(0))->diameter(d0);
	((fea::mesh::sections::Circle*) model.mesh()->section(1))->diameter(d1);
	((fea::mesh::sections::Circle*) model.mesh()->section(2))->diameter(d2);

	//elements
	for(unsigned i = 0; i < 2; i++)
	{
		model.mesh()->add_element(fea::mesh::elements::type::bar, {9 * i + 0, 9 * i + 5}, 0, 1);
		model.mesh()->add_element(fea::mesh::elements::type::bar, {9 * i + 0, 9 * i + 6}, 0, 1);
		model.mesh()->add_element(fea::mesh::elements::type::bar, {9 * i + 0, 9 * i + 7}, 0, 1);
		model.mesh()->add_element(fea::mesh::elements::type::bar, {9 * i + 0, 9 * i + 8}, 0, 1);
		model.mesh()->add_element(fea::mesh::elements::type::bar, {9 * i + 5, 9 * i + 1}, 0, 1);
		model.mesh()->add_element(fea::mesh::elements::type::bar, {9 * i + 6, 9 * i + 2}, 0, 1);
		model.mesh()->add_element(fea::mesh::elements::type::bar, {9 * i + 7, 9 * i + 3}, 0, 1);
		model.mesh()->add_element(fea::mesh::elements::type::bar, {9 * i + 8, 9 * i + 4}, 0, 1);
		model.mesh()->add_element(fea::mesh::elements::type::bar, {9 * i + 5, 9 * i + 6}, 0, 2);
		model.mesh()->add_element(fea::mesh::elements::type::bar, {9 * i + 6, 9 * i + 7}, 0, 2);
		model.mesh()->add_element(fea::mesh::elements::type::bar, {9 * i + 7, 9 * i + 8}, 0, 2);
		model.mesh()->add_element(fea::mesh::elements::type::bar, {9 * i + 8, 9 * i + 5}, 0, 2);
		model.mesh()->add_element(fea::mesh::elements::type::bar, {9 * i + 5, 9 * i + 2}, 0, 2);
		model.mesh()->add_element(fea::mesh::elements::type::bar, {9 * i + 6, 9 * i + 1}, 0, 2);
		model.mesh()->add_element(fea::mesh::elements::type::bar, {9 * i + 6, 9 * i + 3}, 0, 2);
		model.mesh()->add_element(fea::mesh::elements::type::bar, {9 * i + 7, 9 * i + 2}, 0, 2);
		model.mesh()->add_element(fea::mesh::elements::type::bar, {9 * i + 7, 9 * i + 4}, 0, 2);
		model.mesh()->add_element(fea::mesh::elements::type::bar, {9 * i + 8, 9 * i + 3}, 0, 2);
		model.mesh()->add_element(fea::mesh::elements::type::bar, {9 * i + 8, 9 * i + 1}, 0, 2);
		model.mesh()->add_element(fea::mesh::elements::type::bar, {9 * i + 5, 9 * i + 4}, 0, 2);
	}
	for(unsigned i = 0; i < n; i++)
	{
		model.mesh()->add_element(fea::mesh::elements::type::bar, {18 + i, 19 + i}, 0, 0);
	}

	//supports
	for(unsigned i = 0; i < 2; i++)
	{
		for(unsigned j = 0; j < 4; j++)
		{
			model.boundary()->add_support(9 * i + j + 1, fea::mesh::nodes::dof::translation_x);
			model.boundary()->add_support(9 * i + j + 1, fea::mesh::nodes::dof::translation_y);
			model.boundary()->add_support(9 * i + j + 1, fea::mesh::nodes::dof::translation_z);
		}
	}
	for(unsigned i = 0; i < n; i++)
	{
		model.boundary()->add_support(18 + i, fea::mesh::nodes::dof::translation_y);
	}
	
	//dependencies
	model.boundary()->add_dependency(0, fea::mesh::nodes::dof::translation_x, 18, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_dependency(0, fea::mesh::nodes::dof::translation_y, 18, fea::mesh::nodes::dof::translation_y);
	model.boundary()->add_dependency(0, fea::mesh::nodes::dof::translation_z, 18, fea::mesh::nodes::dof::translation_z);
	model.boundary()->add_dependency(9, fea::mesh::nodes::dof::translation_x, 18 + n, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_dependency(9, fea::mesh::nodes::dof::translation_y, 18 + n, fea::mesh::nodes::dof::translation_y);
	model.boundary()->add_dependency(9, fea::mesh::nodes::dof::translation_z, 18 + n, fea::mesh::nodes::dof::translation_z);

	//solver
	model.analysis()->solver(fea::analysis::solvers::type::buckling);
	dynamic_cast<fea::analysis::solvers::Buckling*>(model.analysis()->solver())->tolerance(1e-8);
	dynamic_cast<fea::analysis::solvers::Buckling*>(model.analysis()->solver())->spectre(fea::analysis::solvers::spectre::full);
	dynamic_cast<fea::analysis::solvers::Buckling*>(model.analysis()->solver())->watch_dof(18 + n / 2, fea::mesh::nodes::dof::translation_z);

	//solve
	model.analysis()->solve();

	//save
	model.save();
}
