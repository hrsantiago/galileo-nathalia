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
#include "Mesh/Sections/Generic.h"
#include "Mesh/Sections/Rectangle.h"
#include "Mesh/Elements/Mechanic/Mechanic.h"
#include "Mesh/Materials/Mechanic/Associative/Steel.h"

#include "Boundary/Boundary.h"
#include "Boundary/Cases/Load_Case.h"
#include "Boundary/Loads/Nodes/Node.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Types.h"
#include "Analysis/Solvers/Dynamic_Nonlinear.h"

//ben
#include "benchmarks/mechanic/beam.h"

void tests::beam::dynamic_nonlinear::arc_shallow_clamped(void) 
{
	/*
	Elastic clamped shallow arc subjected to a tip load
	Literature: T. N. Le et al. (2011) - Computer Mechanics - pp. 153 - 161
	 */

	//model
	fea::models::Model model("arc shallow clamped", "benchmarks/beam/dynamic/nonlinear");

	//nodes
	const double R = 10.0;
	const unsigned ne = 60;
	const unsigned nn = ne + 1;
	for(unsigned i = 0; i < nn; i++)
	{
		const double t = (1 + 2 * (double) i / ne) * M_PI / 6;
		const double x = R * cos(t);
		const double y = R * sin(t);
		model.mesh()->add_node(x, y, 0);
	}

	//cells
	model.mesh()->add_cell(fea::mesh::cells::type::beam);
	
	//materials
	model.mesh()->add_material(fea::mesh::materials::type::steel);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->elastic_modulus(210e9);

	//sections
	model.mesh()->add_section(fea::mesh::sections::type::rectangle);
	((fea::mesh::sections::Rectangle*) model.mesh()->section(0))->width(0.10);
	((fea::mesh::sections::Rectangle*) model.mesh()->section(0))->height(0.10);

	//elements
	fea::mesh::elements::Mechanic::geometric(true);
	for(unsigned i = 0; i < ne; i++)
	{
		model.mesh()->add_element(fea::mesh::elements::type::beam2, {i, i + 1});
	}

	//supports
	model.boundary()->add_support(0, fea::mesh::nodes::dof::rotation_z);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_y);
	
	model.boundary()->add_support(ne, fea::mesh::nodes::dof::rotation_z);
	model.boundary()->add_support(ne, fea::mesh::nodes::dof::translation_x);

	//loads
	model.boundary()->add_load_case();
	model.boundary()->load_case(0)->add_load_node(nn - 1, fea::mesh::nodes::dof::translation_y, -40e6);
	model.boundary()->load_case(0)->load_node(0)->function([](double t) { return sin(1e3 * t); });

	//solver
	model.analysis()->solver(fea::analysis::solvers::type::dynamic_nonlinear);
	dynamic_cast<fea::analysis::solvers::Dynamic_Nonlinear*> (model.analysis()->solver())->time_max(0.05);
	dynamic_cast<fea::analysis::solvers::Dynamic_Nonlinear*> (model.analysis()->solver())->step_max(4000);
	dynamic_cast<fea::analysis::solvers::Dynamic_Nonlinear*> (model.analysis()->solver())->time_adjust(true);
	dynamic_cast<fea::analysis::solvers::Dynamic_Nonlinear*> (model.analysis()->solver())->iteration_desired(3);

	//solve
	model.analysis()->solve();
	
	//save
	model.save();
}
