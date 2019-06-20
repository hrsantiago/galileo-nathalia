//std
#include <cmath>

//fea
#include "Models/Model.h"

#include "Mesh/Mesh.h"
#include "Mesh/Nodes/Dofs.h"
#include "Mesh/Cells/Types.h"
#include "Mesh/Sections/Types.h"
#include "Mesh/Elements/Types.h"
#include "Mesh/Cells/Line/Line.h"
#include "Mesh/Materials/Types.h"
#include "Mesh/Elements/Mechanic/Mechanic.h"
#include "Mesh/Materials/Mechanic/Associative/Steel.h"
#include "Mesh/Materials/Mechanic/Associative/Concrete.h"

#include "Boundary/Boundary.h"
#include "Boundary/Cases/Load_Case.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Types.h"
#include "Analysis/Strategies/Types.h"
#include "Analysis/Solvers/Static_Nonlinear.h"

//ben
#include "benchmarks/mechanic/beam.h"

void tests::beam::static_nonlinear::inelastic::concrete::frame_square(void) 
{
	/*
	fea::mesh::materials::Concrete square
	Literature: International Journal of Solids and Structures - S. Bratina et al. (2004) vol. 41 pp. 7181�7207
	 */

	//model
	fea::models::Model model("frame square", "benchmarks/beam/static/nonlinear/inelastic/concrete");

	//nodes
	const double L = 2.13;
	model.mesh()->add_node(0, 0, 0);
	model.mesh()->add_node(L, 0, 0);
	model.mesh()->add_node(L, L, 0);
	model.mesh()->add_node(0, L, 0);

	//cells
	model.mesh()->add_cell(fea::mesh::cells::type::beam);
	
	//materials
	model.mesh()->add_material(fea::mesh::materials::type::concrete);
	((fea::mesh::materials::Concrete*) model.mesh()->material(0))->softening(5.0);
	((fea::mesh::materials::Concrete*) model.mesh()->material(0))->break_strain(0.35e-2);
	((fea::mesh::materials::Concrete*) model.mesh()->material(0))->elastic_modulus(22.4e9);
	((fea::mesh::materials::Concrete*) model.mesh()->material(0))->yield_stress_tension(22.1e4);
	((fea::mesh::materials::Concrete*) model.mesh()->material(0))->yield_stress_biaxial(26.5e6);
	((fea::mesh::materials::Concrete*) model.mesh()->material(0))->yield_stress_compression(22.1e6);
	

	model.mesh()->add_material(fea::mesh::materials::type::steel);
	((fea::mesh::materials::Steel*) model.mesh()->material(1))->break_strain(2e-2);
	((fea::mesh::materials::Steel*) model.mesh()->material(1))->break_stress(390e6);
	((fea::mesh::materials::Steel*) model.mesh()->material(1))->yield_stress(338.9e6);
	((fea::mesh::materials::Steel*) model.mesh()->material(1))->elastic_modulus(202e9);

	model.mesh()->add_material(fea::mesh::materials::type::steel);
	((fea::mesh::materials::Steel*) model.mesh()->material(1))->break_strain(2e-2);
	((fea::mesh::materials::Steel*) model.mesh()->material(1))->break_stress(405e6);
	((fea::mesh::materials::Steel*) model.mesh()->material(1))->yield_stress(403.4e6);
	((fea::mesh::materials::Steel*) model.mesh()->material(1))->elastic_modulus(202e9);

	//sections
//	model.mesh()->add_section(fea::mesh::sections::type::rectangleR);
//	((fea::mesh::sections::RectangleR*) model.mesh()->section(0))->width(15.4e-2);
//	((fea::mesh::sections::RectangleR*) model.mesh()->section(0))->height(10.3e-2);
//	((fea::mesh::sections::RectangleR*) model.mesh()->section(0))->add_bar(2.59e-2, 2.10e-2, 9.84e-3);
//	((fea::mesh::sections::RectangleR*) model.mesh()->section(0))->add_bar(12.8e-2, 2.10e-2, 9.84e-3);
//	((fea::mesh::sections::RectangleR*) model.mesh()->section(0))->add_bar(2.59e-2, 8.20e-2, 9.84e-3);
//	((fea::mesh::sections::RectangleR*) model.mesh()->section(0))->add_bar(12.8e-2, 8.20e-2, 9.84e-3);

//	model.mesh()->add_section(fea::mesh::sections::type::rectangleR);
//	((fea::mesh::sections::RectangleR*) model.mesh()->section(1))->width(15.4e-2);
//	((fea::mesh::sections::RectangleR*) model.mesh()->section(1))->height(8.1e-2);
//	((fea::mesh::sections::RectangleR*) model.mesh()->section(1))->add_bar(2.59e-2, 2.40e-2, 1.78e-2);
//	((fea::mesh::sections::RectangleR*) model.mesh()->section(1))->add_bar(12.8e-2, 2.40e-2, 1.78e-2);
//	((fea::mesh::sections::RectangleR*) model.mesh()->section(1))->add_bar(2.59e-2, 5.70e-2, 1.78e-2);
//	((fea::mesh::sections::RectangleR*) model.mesh()->section(1))->add_bar(12.8e-2, 5.70e-2, 1.78e-2);

	//elements
	fea::mesh::elements::Mechanic::geometric(true);
	fea::mesh::elements::Mechanic::inelastic(true);
	model.mesh()->add_element(fea::mesh::elements::type::beam2, {0, 1}); //reiforcement: material 2
	model.mesh()->add_element(fea::mesh::elements::type::beam2, {1, 2}); //reiforcement: material 1
	model.mesh()->add_element(fea::mesh::elements::type::beam2, {2, 3}); //reiforcement: material 2
	model.mesh()->add_element(fea::mesh::elements::type::beam2, {3, 0}); //reiforcement: material 1

	//supports
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_y);
	model.boundary()->add_support(1, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(1, fea::mesh::nodes::dof::translation_y);

	//loads
	model.boundary()->add_load_case();
	model.boundary()->load_case(0)->add_load_node(2, fea::mesh::nodes::dof::translation_x, -2e1);
	model.boundary()->load_case(0)->add_load_node(2, fea::mesh::nodes::dof::translation_y, -1e3 + 1e1);
	model.boundary()->load_case(0)->add_load_node(3, fea::mesh::nodes::dof::translation_y, -1e3 - 1e1);

	//solver
	model.analysis()->solver(fea::analysis::solvers::type::static_nonlinear);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->step_max(300);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_max(300);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_predictor(5.0);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_increment_max(5.0);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->strategy(fea::analysis::strategies::type::arc_length_cylindric);

	//solve
	model.analysis()->solve();
	
	//save
	model.save();
}
