//std
#include <cmath>

//mat
#include "misc/defs.h"

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

static unsigned index_node(unsigned i, unsigned j, unsigned k, unsigned nr, unsigned nt)
{
	return nt * (nr + 1) * i + (nr + 1) * j + k;
}
static unsigned index_element(unsigned i, unsigned j, unsigned k, unsigned nr, unsigned nt)
{
	return nt * nr * i + nr * j + k;
}

void tests::solid::static_linear::hinge(void)
{
	//model
	fea::models::Model model("hinge", "benchmarks/solid/static/linear");

	//parameters
	const double r = 0.50e0;
	const double t = 1.00e0;
	const double h = 1.00e0;
	const double v = 0.25e0;
	const double E = 1.50e9;

	//mesh
	const unsigned nt = 50;
	const unsigned nr = 10;
	const unsigned nz = 10;

	//nodes
	for(unsigned i = 0; i <= nz; i++)
	{
		for(unsigned j = 1; j <= nt; j++)
		{
			for(unsigned k = 0; k <= nr; k++)
			{
				const double z = i * h / nz;
				const double l = r + k * t / nr;
				const double x = l * cos(2 * j * M_PI / nt);
				const double y = l * sin(2 * j * M_PI / nt);
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
		for(unsigned j = 0; j < nt; j++) 
		{
			for(unsigned k = 0; k < nr; k++)
			{
				const unsigned n0 = index_node(i + 0, j + 0, k + 0, nr, nt);
				const unsigned n1 = index_node(i + 0, j + 0, k + 1, nr, nt);
				const unsigned n2 = index_node(i + 0, j + 1 != nt ? j + 1 : 0, k + 1, nr, nt);
				const unsigned n3 = index_node(i + 0, j + 1 != nt ? j + 1 : 0, k + 0, nr, nt);
				const unsigned n4 = index_node(i + 1, j + 0, k + 0, nr, nt);
				const unsigned n5 = index_node(i + 1, j + 0, k + 1, nr, nt);
				const unsigned n6 = index_node(i + 1, j + 1 != nt ? j + 1 : 0, k + 1, nr, nt);
				const unsigned n7 = index_node(i + 1, j + 1 != nt ? j + 1 : 0, k + 0, nr, nt);
				model.mesh()->add_element(fea::mesh::elements::type::solid, {n0, n1, n2, n3, n4, n5, n6, n7});
			}
		}
	}

	//supports
	model.boundary()->add_support((nr + 1) * (nt - 1), fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support((nr + 1) * (nt - 1), fea::mesh::nodes::dof::translation_y);
	model.boundary()->add_support((nr + 1) * (nt - 1) + nr, fea::mesh::nodes::dof::translation_y);
	for(unsigned i = 0; i < nt; i++) 
	{
		for(unsigned j = 0; j <= nr; j++)
		{
			model.boundary()->add_support(index_node(0, i, j, nr, nt), fea::mesh::nodes::dof::translation_z);
		}
	}

	//loads
	model.boundary()->add_load_case();
	model.boundary()->load_case(0)->add_load_element(fea::boundary::loads::type::solid_face, { }, E);
	((fea::boundary::loads::Solid_Face*) model.boundary()->load_case(0)->load_element(0))->face(5);
	((fea::boundary::loads::Solid_Face*) model.boundary()->load_case(0)->load_element(0))->direction(0, M_PI / 2);
	for(unsigned i = 0; i < nt; i++)
	{
		for(unsigned j = 0; j < nr; j++)
		{
			model.boundary()->load_case(0)->load_element(0)->add_element(index_element(nz - 1, i, j, nr, nt));
		}
	}
			
	//solver
	model.analysis()->solver(fea::analysis::solvers::type::static_linear);
	model.analysis()->solver()->watch_dof(index_node(nz, 0, 0, nr, nt), fea::mesh::nodes::dof::translation_z);

	//solve
	model.analysis()->solve();
	
	//save
	model.save();
}
