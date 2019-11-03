//std
#include <cmath>

//fea
#include "Model/Model.h"

#include "Mesh/Mesh.h"
#include "Mesh/Nodes/Dofs.h"
#include "Mesh/Cells/Types.h"
#include "Mesh/Elements/Types.h"
#include "Mesh/Materials/Types.h"

#include "Boundary/Boundary.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Types.h"

//ben
#include "benchmarks/heat/heat.h"

void tests::heat::static_linear::plate(void) 
{
	//model
	fea::models::Model model("plate", "benchmarks/heat/static/linear");

	//nodes
	const double w = 1.0;
	const double h = 0.2;
	const unsigned ny = 21;
	const unsigned nx = 21;
	for(unsigned i = 0; i < nx; i++) 
	{
		const double x = w * i / (nx - 1);
		for(unsigned j = 0; j < ny; j++) 
		{
			const double y = h * j / (ny - 1);
			model.mesh()->add_node(x, y, 0);
		}
	}

	//cells
	model.mesh()->add_cell(fea::mesh::cells::type::quad4);

	//materials
	model.mesh()->add_material(fea::mesh::materials::type::heat);

	//elements
	for(unsigned i = 0; i < nx - 1; i++) 
	{
		for(unsigned j = 0; j < ny - 1; j++) 
		{
			const unsigned n0 = ny * (i + 0) + j + 0;
			const unsigned n1 = ny * (i + 1) + j + 0;
			const unsigned n2 = ny * (i + 1) + j + 1;
			const unsigned n3 = ny * (i + 0) + j + 1;
			model.mesh()->add_element(fea::mesh::elements::type::heat, { n0, n1, n2, n3 });
		}
	}

	//supports
	for(unsigned i = 0; i < ny; i++) 
	{
		const std::function<double(double)> f = [i, ny] (double) { return 5 * sin(i * M_PI / (ny - 1)); };
		model.boundary()->add_support(i, fea::mesh::nodes::dof::temperature, f);
		model.boundary()->add_support(ny * (nx - 1) + i, fea::mesh::nodes::dof::temperature, f);
	}

	//solver
	model.analysis()->solver(fea::analysis::solvers::type::static_linear);

	//solve
	model.analysis()->solve();
	
	//save
	model.save();
}
