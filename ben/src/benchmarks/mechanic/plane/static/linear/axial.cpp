//fea
#include "Models/Model.h"

#include "Mesh/Mesh.h"
#include "Mesh/Nodes/Dofs.h"
#include "Mesh/Cells/Types.h"
#include "Mesh/Elements/Types.h"
#include "Mesh/Materials/Types.h"
#include "Mesh/Cells/Plane/Plane.h"
#include "Mesh/Materials/Mechanic/Associative/Steel.h"

#include "Boundary/Boundary.h"
#include "Boundary/Loads/Types.h"
#include "Boundary/Cases/Load_Case.h"
#include "Boundary/Loads/Elements/Mechanic/Plane/Plane_Edge.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Types.h"
#include "Analysis/Solvers/Static_Linear.h"

//ben
#include "benchmarks/mechanic/plane.h"

void tests::plane::static_linear::axial(void) 
{
	//model
	fea::models::Model model("axial", "benchmarks/plane/static/linear");

	//nodes
	const double w = 1.00;
	const double h = 0.10;
	const double t = 0.10;
	const double v = 0.25;
	const double E = 200e9;
	const double A = h * t;
	const double P = E * A / w;
	const double s = P / A;
	const unsigned nx = 1;
	const unsigned ny = 1;
	for(unsigned i = 0; i <= nx; i++) 
	{
		for(unsigned j = 0; j <= ny; j++) 
		{
			const double x = w * i / nx;
			const double y = h * j / ny;
			model.mesh()->add_node(x, y, 0);
		}
	}

	//cells
	model.mesh()->add_cell(fea::mesh::cells::type::quad4);
	((fea::mesh::cells::Plane*) model.mesh()->cell(0))->thickness(t);

	//materials
	model.mesh()->add_material(fea::mesh::materials::type::steel);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->poisson_ratio(v);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->elastic_modulus(E);

	//elements
	for(unsigned i = 0; i < nx; i++) 
	{
		for(unsigned j = 0; j < ny; j++) 
		{
			const unsigned n0 = (ny + 1) * (i + 0) + j + 0;
			const unsigned n1 = (ny + 1) * (i + 1) + j + 0;
			const unsigned n2 = (ny + 1) * (i + 1) + j + 1;
			const unsigned n3 = (ny + 1) * (i + 0) + j + 1;
			model.mesh()->add_element(fea::mesh::elements::type::plane, {n0, n1, n2, n3});
		}
	}

	//supports
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_y);
	for(unsigned i = 0; i <= ny; i++) 
	{
		model.boundary()->add_support(i, fea::mesh::nodes::dof::translation_x);
	}

	//loads
	model.boundary()->add_load_case();
	model.boundary()->load_case(0)->add_load_element(fea::boundary::loads::type::plane_edge);
	for(unsigned i = 0; i < ny; i++)
	{
		model.boundary()->load_case(0)->load_element(0)->add_element(ny * (nx - 1) + i);
	}
	((fea::boundary::loads::Plane_Edge*) model.boundary()->load_case(0)->load_element(0))->edge(1);
	((fea::boundary::loads::Plane_Edge*) model.boundary()->load_case(0)->load_element(0))->value(s);

	//solver
	model.analysis()->solver(fea::analysis::solvers::type::static_linear);
	model.analysis()->solver()->watch_dof((ny + 1) * (nx + 1) - 1, fea::mesh::nodes::dof::translation_x);

	//solve
	model.analysis()->solve();
	
	//save
	model.save();
}
