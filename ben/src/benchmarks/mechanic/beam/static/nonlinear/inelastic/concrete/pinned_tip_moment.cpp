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
#include "Mesh/Sections/Rectangle.h"
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

void tests::beam::static_nonlinear::inelastic::concrete::pinned_tip_moment(void) 
{
	/*
	fea::mesh::materials::Concrete pinned beam subjected to a moment in both ends
	Literature: ULB Phd Thesis B. S. Iribarren (2011) pp. 32
	 */

	//model
	fea::models::Model model("pinned tip moment", "benchmarks/beam/static/nonlinear/inelastic/concrete");

	//nodes
	model.mesh()->add_node(0, 0, 0);
	model.mesh()->add_node(2, 0, 0);

	//cells
	model.mesh()->add_cell(fea::mesh::cells::type::beam);
	
	//materials
	model.mesh()->add_material(fea::mesh::materials::type::concrete);
	((fea::mesh::materials::Concrete*) model.mesh()->material(0))->softening(6.7e3);
	((fea::mesh::materials::Concrete*) model.mesh()->material(0))->break_strain(0.35e-2);
	((fea::mesh::materials::Concrete*) model.mesh()->material(0))->elastic_modulus(17e9);
	((fea::mesh::materials::Concrete*) model.mesh()->material(0))->yield_stress_tension(3.25e6);
	((fea::mesh::materials::Concrete*) model.mesh()->material(0))->yield_stress_compression(37.9e6);

	model.mesh()->add_material(fea::mesh::materials::type::steel);
	((fea::mesh::materials::Steel*) model.mesh()->material(1))->break_strain(4e-2);
	((fea::mesh::materials::Steel*) model.mesh()->material(1))->break_stress(525e6);
	((fea::mesh::materials::Steel*) model.mesh()->material(1))->yield_stress(500e6);
	((fea::mesh::materials::Steel*) model.mesh()->material(1))->elastic_modulus(200e9);

	//sections
//	model.mesh()->add_section(fea::mesh::sections::type::rectangleR);
//	((fea::mesh::sections::RectangleR*) model.mesh()->section(0))->width(45e-2);
//	((fea::mesh::sections::RectangleR*) model.mesh()->section(0))->height(60e-2);
//	((fea::mesh::sections::RectangleR*) model.mesh()->section(0))->overlap(5e-2);
//	((fea::mesh::sections::RectangleR*) model.mesh()->section(0))->add_bar(0.060, 0.060, 20e-3);
//	((fea::mesh::sections::RectangleR*) model.mesh()->section(0))->add_bar(0.390, 0.060, 20e-3);
//	((fea::mesh::sections::RectangleR*) model.mesh()->section(0))->add_bar(0.082, 0.534, 32e-3);
//	((fea::mesh::sections::RectangleR*) model.mesh()->section(0))->add_bar(0.225, 0.534, 32e-3);
//	((fea::mesh::sections::RectangleR*) model.mesh()->section(0))->add_bar(0.368, 0.534, 32e-3);

//	model.mesh()->add_section(fea::mesh::sections::type::rectangleR);
//	((fea::mesh::sections::RectangleR*) model.mesh()->section(1))->width(45e-2);
//	((fea::mesh::sections::RectangleR*) model.mesh()->section(1))->height(60e-2);
//	((fea::mesh::sections::RectangleR*) model.mesh()->section(1))->overlap(5e-2);
//	((fea::mesh::sections::RectangleR*) model.mesh()->section(1))->add_bar(0.082, 0.066, 32e-3);
//	((fea::mesh::sections::RectangleR*) model.mesh()->section(1))->add_bar(0.368, 0.066, 32e-3);
//	((fea::mesh::sections::RectangleR*) model.mesh()->section(1))->add_bar(0.082, 0.534, 32e-3);
//	((fea::mesh::sections::RectangleR*) model.mesh()->section(1))->add_bar(0.225, 0.534, 32e-3);
//	((fea::mesh::sections::RectangleR*) model.mesh()->section(1))->add_bar(0.368, 0.534, 32e-3);

//	model.mesh()->add_section(fea::mesh::sections::type::rectangleR);
//	((fea::mesh::sections::RectangleR*) model.mesh()->section(2))->width(45e-2);
//	((fea::mesh::sections::RectangleR*) model.mesh()->section(2))->height(60e-2);
//	((fea::mesh::sections::RectangleR*) model.mesh()->section(2))->overlap(5e-2);
//	((fea::mesh::sections::RectangleR*) model.mesh()->section(2))->add_bar(0.066, 0.066, 32e-3);
//	((fea::mesh::sections::RectangleR*) model.mesh()->section(2))->add_bar(0.225, 0.066, 32e-3);
//	((fea::mesh::sections::RectangleR*) model.mesh()->section(2))->add_bar(0.384, 0.066, 32e-3);
//	((fea::mesh::sections::RectangleR*) model.mesh()->section(2))->add_bar(0.066, 0.534, 32e-3);
//	((fea::mesh::sections::RectangleR*) model.mesh()->section(2))->add_bar(0.225, 0.534, 32e-3);
//	((fea::mesh::sections::RectangleR*) model.mesh()->section(2))->add_bar(0.384, 0.534, 32e-3);

	//elements
	fea::mesh::elements::Mechanic::geometric(true);
	fea::mesh::elements::Mechanic::inelastic(true);
	model.mesh()->add_element(fea::mesh::elements::type::beam2, {0, 1});

	//supports
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_y);
	model.boundary()->add_support(1, fea::mesh::nodes::dof::translation_y);

	//loads
	model.boundary()->add_load_case();
	model.boundary()->load_case(0)->add_load_node(0, fea::mesh::nodes::dof::rotation_z, -600e3);
	model.boundary()->load_case(0)->add_load_node(1, fea::mesh::nodes::dof::rotation_z, +600e3);

	//solver
	model.analysis()->solver(fea::analysis::solvers::type::static_nonlinear);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->step_max(1000);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_max(2.00);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->strategy(fea::analysis::strategies::type::minimal_norm);

	//solve
	model.analysis()->solve();
	
	//save
	model.save();
}
