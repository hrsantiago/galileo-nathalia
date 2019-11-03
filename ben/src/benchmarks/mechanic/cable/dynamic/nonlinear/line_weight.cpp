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
#include "Mesh/Elements/Mechanic/Mechanic.h"
#include "Mesh/Elements/Mechanic/Frame/Bar.h"
#include "Mesh/Materials/Mechanic/Associative/Steel.h"

#include "Boundary/Boundary.h"
#include "Boundary/Cases/Load_Case.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Types.h"
#include "Analysis/Solvers/Dynamic_Nonlinear.h"

//ben
#include "benchmarks/mechanic/cable.h"

void tests::cable::dynamic_nonlinear::line_weight(void) 
{
	//model
	fea::models::Model model("line weight", "benchmarks/cable/dynamic/nonlinear");

	//parameters
	const unsigned n = 20;
	
	const double a = 2.00e+0;
	const double h = 0.00e+1;
	const double w = 1.00e+2;
	const double d = 1.00e-1;
	const double E = 2.00e11;
	const double r = 7.85e+3;
	
	//nodes
	model.mesh()->add_node(0, 0, 0);
	model.mesh()->add_node(w, h, 0);

	//cells
	model.mesh()->add_cell(fea::mesh::cells::type::bar);

	//materials
	model.mesh()->add_material(fea::mesh::materials::type::steel);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->specific_mass(r);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->elastic_modulus(E);

	//sections
	model.mesh()->add_section(fea::mesh::sections::type::circle);
	((fea::mesh::sections::Circle*) model.mesh()->section(0))->diameter(d);

	//elements
	model.mesh()->add_element(fea::mesh::elements::type::bar, {0, 1});
	((fea::mesh::elements::Bar*) model.mesh()->element(0))->cable(true);
	((fea::mesh::elements::Bar*) model.mesh()->element(0))->length(a * sqrt(w * w + h * h) / n);
	
	//refine
	fea::mesh::cells::Line::refine(0, n);

	//supports
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_y);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_z);
	model.boundary()->add_support(1, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(1, fea::mesh::nodes::dof::translation_y);
	model.boundary()->add_support(1, fea::mesh::nodes::dof::translation_z);
	for(unsigned i = 0; i <= n; i++)
	{
		model.boundary()->add_support(i, fea::mesh::nodes::dof::translation_x, 0, 1.00e+1, 0);
		model.boundary()->add_support(i, fea::mesh::nodes::dof::translation_y, 0, 1.00e+1, 0);
	}

	//loads
	model.boundary()->add_self_weight("gravity", fea::mesh::nodes::dof::translation_y, 9.81);

	//solver
	fea::mesh::elements::Mechanic::geometric(true);
	model.analysis()->solver(fea::analysis::solvers::type::dynamic_nonlinear);
	dynamic_cast<fea::analysis::solvers::Dynamic_Nonlinear*> (model.analysis()->solver())->time_max(30);
	dynamic_cast<fea::analysis::solvers::Dynamic_Nonlinear*> (model.analysis()->solver())->step_max(20000);
	dynamic_cast<fea::analysis::solvers::Dynamic_Nonlinear*> (model.analysis()->solver())->watch_dof(n / 2, fea::mesh::nodes::dof::translation_y);
	dynamic_cast<fea::analysis::solvers::Dynamic_Nonlinear*> (model.analysis()->solver())->integration(fea::analysis::solvers::integration::runge_kutta);

	//solve
	model.analysis()->solve();
	
	//save
	model.save();
}
