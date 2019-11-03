//std
#include <cmath>

//fea
#include "Model/Model.h"

#include "Mesh/Mesh.h"
#include "Mesh/Nodes/Dofs.h"
#include "Mesh/Cells/Types.h"
#include "Mesh/Elements/Types.h"
#include "Mesh/Materials/Types.h"
#include "Mesh/Materials/Mechanic/Associative/Steel.h"

#include "Boundary/Boundary.h"
#include "Boundary/Loads/Types.h"
#include "Boundary/Cases/Load_Case.h"
#include "Boundary/Loads/Elements/Mechanic/Solid/Solid_Face.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Types.h"
#include "Analysis/Solvers/Static_Linear.h"

//ben
#include "benchmarks/mechanic/solid.h"

static unsigned index_node(unsigned i, unsigned j, unsigned k, unsigned nx, unsigned ny)
{
	return (ny + 1) * (nx + 1) * i + (nx + 1) * j + k;
}
static unsigned index_element(unsigned i, unsigned j, unsigned k, unsigned nx, unsigned ny)
{
	return ny * nx * i + nx * j + k;
}

void tests::solid::static_linear::axial(void)
{
	//model
	fea::models::Model model("axial", "benchmarks/solid/static/linear");

	//parameters
	const double w = 1.00;
	const double h = 1.00;
	const double t = 1.00;
	const double v = 0.25;
	const double E = 200e9;

	//mesh
	const unsigned nx = 10;
	const unsigned ny = 10;
	const unsigned nz = 10;

	//nodes
	for(unsigned i = 0; i <= nz; i++)
	{
		for(unsigned j = 0; j <= ny; j++)
		{
			for(unsigned k = 0; k <= nx; k++)
			{
				const double x = w * k / nx;
				const double y = t * j / ny;
				const double z = h * i / nz;
				model.mesh()->add_node(x, y, z);
			}
		}
	}

	//cells
	model.mesh()->add_cell(fea::mesh::cells::type::brick8);

	//materials
	model.mesh()->add_material(fea::mesh::materials::type::steel);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->poisson_ratio(v);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->elastic_modulus(E);

	//elements
	for(unsigned i = 0; i < nz; i++) 
	{
		for(unsigned j = 0; j < ny; j++) 
		{
			for(unsigned k = 0; k < nx; k++)
			{
				const unsigned n0 = index_node(i + 0, j + 0, k + 0, nx, ny);
				const unsigned n1 = index_node(i + 0, j + 0, k + 1, nx, ny);
				const unsigned n2 = index_node(i + 0, j + 1, k + 1, nx, ny);
				const unsigned n3 = index_node(i + 0, j + 1, k + 0, nx, ny);
				const unsigned n4 = index_node(i + 1, j + 0, k + 0, nx, ny);
				const unsigned n5 = index_node(i + 1, j + 0, k + 1, nx, ny);
				const unsigned n6 = index_node(i + 1, j + 1, k + 1, nx, ny);
				const unsigned n7 = index_node(i + 1, j + 1, k + 0, nx, ny);
				model.mesh()->add_element(fea::mesh::elements::type::solid, {n0, n1, n2, n3, n4, n5, n6, n7});
			}
		}
	}

	//supports
	model.boundary()->add_support( 0, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support( 0, fea::mesh::nodes::dof::translation_y);
	model.boundary()->add_support(nx, fea::mesh::nodes::dof::translation_y);
	for(unsigned i = 0; i <= ny; i++) 
	{
		for(unsigned j = 0; j <= nx; j++)
		{
			model.boundary()->add_support(index_node(0, i, j, nx, ny), fea::mesh::nodes::dof::translation_z);
		}
	}

	//loads
	model.boundary()->add_load_case();
	model.boundary()->load_case(0)->add_load_element(fea::boundary::loads::type::solid_face, { }, E);
	((fea::boundary::loads::Solid_Face*) model.boundary()->load_case(0)->load_element(0))->face(5);
	((fea::boundary::loads::Solid_Face*) model.boundary()->load_case(0)->load_element(0))->direction(0, M_PI / 2);
	for(unsigned i = 0; i < ny; i++)
	{
		for(unsigned j = 0; j < nx; j++)
		{
			model.boundary()->load_case(0)->load_element(0)->add_element(index_element(nz - 1, i, j, nx, ny));
		}
	}

	//solver
	model.analysis()->solver(fea::analysis::solvers::type::static_linear);
	model.analysis()->solver()->watch_dof(index_node(nz, 0, 0, nx, ny), fea::mesh::nodes::dof::translation_z);

	//solve
	model.analysis()->solve();
	
	//save
	model.save();
}
