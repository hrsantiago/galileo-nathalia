//std
#include <cmath>

//fea
#include "Model/Model.h"

#include "Mesh/Mesh.h"
#include "Mesh/Nodes/Dofs.h"
#include "Mesh/Cells/Types.h"
#include "Mesh/Sections/Types.h"
#include "Mesh/Elements/Types.h"
#include "Mesh/Materials/Types.h"
#include "Mesh/Sections/Rectangle.h"
#include "Mesh/Elements/Mechanic/Mechanic.h"
#include "Mesh/Materials/Mechanic/Associative/Steel.h"

#include "Boundary/Boundary.h"
#include "Boundary/Cases/Load_Case.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Types.h"
#include "Analysis/Strategies/Types.h"
#include "Analysis/Solvers/Static_Nonlinear.h"

//ben
#include "benchmarks/mechanic/bar.h"

void tests::bar::static_nonlinear::inelastic_tip_force(void) 
{
	/*
	Plastic pinned truss subjected to a tip force
	Literature: A. Ochsner - Elasto-Plasticity of Frame Structure Elements (2014) pp. 46
	 */

	//parameters
	const double d = +1.00e-01;
	const double l = +1.00e+00;
	const double E = +2.00e+11;
	const double K = +2.00e+11;
	const double s = +4.00e+08;
	
	const double A = d * d;

	//model
	fea::models::Model model("inelastic tip force", "benchmarks/bar/static/nonlinear");

	//nodes
	model.mesh()->add_node(0, 0, 0);
	model.mesh()->add_node(l, 0, 0);

	//cells
	model.mesh()->add_cell(fea::mesh::cells::type::bar);

	//materials
	model.mesh()->add_material(fea::mesh::materials::type::steel);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->yield_stress(s);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->plastic_modulus(K);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->elastic_modulus(E);

	//sections
	model.mesh()->add_section(fea::mesh::sections::type::rectangle);
	((fea::mesh::sections::Rectangle*) model.mesh()->section(0))->width(d);
	((fea::mesh::sections::Rectangle*) model.mesh()->section(0))->height(d);

	//elements
	fea::mesh::elements::Mechanic::inelastic(true);
	model.mesh()->add_element(fea::mesh::elements::type::bar, {0, 1});

	//supports
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_y);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_z);
	model.boundary()->add_support(1, fea::mesh::nodes::dof::translation_y);
	model.boundary()->add_support(1, fea::mesh::nodes::dof::translation_z);

	//loads
	model.boundary()->add_load_case(1, fea::mesh::nodes::dof::translation_x, s * A);

	//solver
	model.analysis()->solver(fea::analysis::solvers::type::static_nonlinear);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->step_max(200);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_max(2e0);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_predictor(0.01);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->watch_dof(1, fea::mesh::nodes::dof::translation_x);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->strategy(fea::analysis::strategies::type::minimal_norm);

	//solve
	model.analysis()->solve();
	
	//save
	model.save();
}
