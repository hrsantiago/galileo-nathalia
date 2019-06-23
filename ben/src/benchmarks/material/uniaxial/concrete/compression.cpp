//fea
#include "Model/Model.h"

#include "Mesh/Mesh.h"
#include "Mesh/Nodes/Dofs.h"
#include "Mesh/Sections/Types.h"
#include "Mesh/Materials/Types.h"
#include "Mesh/Sections/Rectangle.h"
#include "Mesh/Elements/Mechanic/Mechanic.h"
#include "Mesh/Materials/Mechanic/Associative/Concrete.h"

#include "Boundary/Boundary.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Types.h"
#include "Analysis/Strategies/Types.h"
#include "Analysis/Solvers/Static_Nonlinear.h"

//ben
#include "benchmarks/material/material.h"

void tests::materials::uniaxial::concrete_compression(void)
{
	//model
	fea::models::Model model("Concrete uniaxial compression");

	//nodes
	model.mesh()->add_node(0, 0, 0);
	model.mesh()->add_node(1, 0, 0);

	//materials
	model.mesh()->add_material(fea::mesh::materials::type::concrete);
	((fea::mesh::materials::Concrete*) model.mesh()->material(0))->softening(6.7e3);
	((fea::mesh::materials::Concrete*) model.mesh()->material(0))->break_strain(0.35e-2);
	((fea::mesh::materials::Concrete*) model.mesh()->material(0))->elastic_modulus(17e9);
	((fea::mesh::materials::Concrete*) model.mesh()->material(0))->yield_stress_tension(3.25e6);
	((fea::mesh::materials::Concrete*) model.mesh()->material(0))->yield_stress_biaxial(37.9e6);
	((fea::mesh::materials::Concrete*) model.mesh()->material(0))->yield_stress_compression(37.9e6);

	//sections
	model.mesh()->add_section(fea::mesh::sections::type::rectangle);
	((fea::mesh::sections::Rectangle*) model.mesh()->section(0))->width(0.10);
	((fea::mesh::sections::Rectangle*) model.mesh()->section(0))->height(0.10);

	//elements
	fea::mesh::elements::Mechanic::inelastic(true);
	//model.add_frame(fea::mesh::elements::type::bar, 0, 1, 0, 0);

	//supports
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_y);
	model.boundary()->add_support(1, fea::mesh::nodes::dof::translation_y);

	//loads
	model.boundary()->add_load_case(1, fea::mesh::nodes::dof::translation_x, -37.9e4);

	//solver
	model.analysis()->solver(fea::analysis::solvers::type::static_nonlinear);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->step_max(100);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_max(2e0);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_predictor(0.025);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->strategy(fea::analysis::strategies::type::arc_length_linear);

	//save
	model.save();

	//solve
	model.analysis()->solve();
}
