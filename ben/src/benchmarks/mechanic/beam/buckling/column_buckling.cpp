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
#include "Mesh/Materials/Types.h"
#include "Mesh/Sections/Rectangle.h"
#include "Mesh/Elements/Mechanic/Mechanic.h"
#include "Mesh/Materials/Mechanic/Associative/Steel.h"

#include "Boundary/Boundary.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Types.h"
#include "Analysis/Solvers/Buckling.h"

//ben
#include "benchmarks/mechanic/beam.h"

void tests::beam::buckling::column_buckling(void) 
{
	//parameters
	const double d = 1.00e-01;
	const double l = 1.00e+00;
	const double E = 2.00e+11;

	const double I = pow(d, 4) / 12;

	//model
	fea::models::Model model("column buckling", "benchmarks/beam/buckling");

	//nodes
	model.mesh()->add_node(0, 0, 0);
	model.mesh()->add_node(l, 0, 0);

	//cells
	model.mesh()->add_cell(fea::mesh::cells::type::beam);

	//materials
	model.mesh()->add_material(fea::mesh::materials::type::steel);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->elastic_modulus(E);

	//sections
	model.mesh()->add_section(fea::mesh::sections::type::rectangle);
	((fea::mesh::sections::Rectangle*) model.mesh()->section(0))->width(d);
	((fea::mesh::sections::Rectangle*) model.mesh()->section(0))->height(d);

	//elements
	model.mesh()->add_element(fea::mesh::elements::type::beam3, {0, 1});

	//refine
	fea::mesh::cells::Line::refine(0, 10);

	//supports
//	model.boundary()->add_support(0, fea::mesh::nodes::dof::rotation_x);
//	model.boundary()->add_support(0, fea::mesh::nodes::dof::rotation_y);
//	model.boundary()->add_support(0, fea::mesh::nodes::dof::rotation_z);
//	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_x);
//	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_y);
//	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_z);

	//solver
	model.analysis()->solver(fea::analysis::solvers::type::buckling);
	model.analysis()->solver()->watch_dof(1, fea::mesh::nodes::dof::translation_y);
	dynamic_cast<fea::analysis::solvers::Buckling*>(model.analysis()->solver())->scale(0.1);
	dynamic_cast<fea::analysis::solvers::Buckling*>(model.analysis()->solver())->spectre(fea::analysis::solvers::spectre::full);

	//solve
	model.analysis()->solve();
	
	//save
	model.save();
}
