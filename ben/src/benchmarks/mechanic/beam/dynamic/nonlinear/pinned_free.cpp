//std
#include <cmath>

//mat
#include "misc/defs.h"

//fea
#include "Model/Model.h"

#include "Mesh/Mesh.h"
#include "Mesh/Nodes/Dofs.h"
#include "Mesh/Nodes/Node.h"
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
#include "Analysis/Solvers/Dynamic_Nonlinear.h"

//ben
#include "benchmarks/mechanic/beam.h"

void tests::beam::dynamic_nonlinear::pinned_free(void) 
{
	/*
	Elastic pinned beam subjected to free vibration
	Literature: T. N. Le et al. (2011) - Computer Mechanics - pp. 153 - 161
	 */

	//model
	fea::models::Model model("pinned free", "benchmarks/beam/dynamic/nonlinear");

	//parameters
	const double a = 1.00;
	const double L = 30.0;
	const double A = 0.20;
	const double r = 7.85e3;
	const double E = 2.10e11;
	const unsigned m = 3;

	//nodes
	model.mesh()->add_node(0, 0, 0);
	model.mesh()->add_node(L, 0, 0);

	//cells
	model.mesh()->add_cell(fea::mesh::cells::type::beam);
	
	//materials
	model.mesh()->add_material(fea::mesh::materials::type::steel);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->specific_mass(r);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->elastic_modulus(E);

	//sections
	model.mesh()->add_section(fea::mesh::sections::type::rectangle);
	((fea::mesh::sections::Rectangle*) model.mesh()->section(0))->width(a);
	((fea::mesh::sections::Rectangle*) model.mesh()->section(0))->height(a);

	//elements
	fea::mesh::elements::Mechanic::geometric(true);
	model.mesh()->add_element(fea::mesh::elements::type::beam2, {0, 1});
	
	//refine
	fea::mesh::cells::Line::refine(0, 30);

	//supports
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_y);
	model.boundary()->add_support(1, fea::mesh::nodes::dof::translation_y);

	//initials
	for(unsigned i = 2; i < model.mesh()->nodes(); i++)
	{
		const double x = model.mesh()->node(i)->coordinates()[0];
		model.boundary()->add_initial(i, fea::mesh::nodes::dof::translation_y, A * sin(m * M_PI * x / L), 0);
	}
	
	//loads
	model.boundary()->add_load_case();
	
	//solver
	model.analysis()->solver(fea::analysis::solvers::type::dynamic_nonlinear);
	dynamic_cast<fea::analysis::solvers::Dynamic_Nonlinear*> (model.analysis()->solver())->tolerance(1e-2);
	dynamic_cast<fea::analysis::solvers::Dynamic_Nonlinear*> (model.analysis()->solver())->step_max(10000);
	dynamic_cast<fea::analysis::solvers::Dynamic_Nonlinear*> (model.analysis()->solver())->time_max(1.3e-1);
	dynamic_cast<fea::analysis::solvers::Dynamic_Nonlinear*> (model.analysis()->solver())->time_adjust(true);
	dynamic_cast<fea::analysis::solvers::Dynamic_Nonlinear*> (model.analysis()->solver())->iteration_desired(5);

	//solve
	model.analysis()->solve();
	
	//save
	model.save();
}
