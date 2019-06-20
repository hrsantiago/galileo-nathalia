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

void tests::beam::static_nonlinear::inelastic::concrete::column_buckling(void) 
{
	/*
	fea::mesh::materials::Concrete column buckling
	Literature: International Journal of Solids and Structures - S. Bratina et al. (2004) vol. 41 pp. 7181�7207
	 */

	//model
	fea::models::Model model("column buckling", "benchmarks/beam/static/nonlinear/inelastic/concrete");

	//nodes
	model.mesh()->add_node(0.00, 0.00, 0.00);
	model.mesh()->add_node(0.00, 2.25, 0.00);

	//cells
	model.mesh()->add_cell(fea::mesh::cells::type::beam);
	
	//materials
	model.mesh()->add_material(fea::mesh::materials::type::concrete);
	((fea::mesh::materials::Concrete*) model.mesh()->material(0))->softening(5.0);
	((fea::mesh::materials::Concrete*) model.mesh()->material(0))->break_strain(0.35e-2);
	((fea::mesh::materials::Concrete*) model.mesh()->material(0))->elastic_modulus(28.6e9);
	((fea::mesh::materials::Concrete*) model.mesh()->material(0))->yield_stress_tension(38.3e4);
	((fea::mesh::materials::Concrete*) model.mesh()->material(0))->yield_stress_biaxial(46.0e6);
	((fea::mesh::materials::Concrete*) model.mesh()->material(0))->yield_stress_compression(38.3e6);

	model.mesh()->add_material(fea::mesh::materials::type::steel);
	((fea::mesh::materials::Steel*) model.mesh()->material(1))->break_strain(2e-2);
	((fea::mesh::materials::Steel*) model.mesh()->material(1))->break_stress(466e6);
	((fea::mesh::materials::Steel*) model.mesh()->material(1))->yield_stress(465e6);
	((fea::mesh::materials::Steel*) model.mesh()->material(1))->elastic_modulus(200e9);

	//sections
//	model.mesh()->add_section(fea::mesh::sections::type::rectangleR);
//	((fea::mesh::sections::RectangleR*) model.mesh()->section(0))->width(0.20);
//	((fea::mesh::sections::RectangleR*) model.mesh()->section(0))->height(0.15);
//	((fea::mesh::sections::RectangleR*) model.mesh()->section(0))->add_bar(0.026, 0.026, 0.012);
//	((fea::mesh::sections::RectangleR*) model.mesh()->section(0))->add_bar(0.174, 0.026, 0.012);
//	((fea::mesh::sections::RectangleR*) model.mesh()->section(0))->add_bar(0.026, 0.124, 0.012);
//	((fea::mesh::sections::RectangleR*) model.mesh()->section(0))->add_bar(0.174, 0.124, 0.012);

	//elements
	fea::mesh::elements::Mechanic::geometric(true);
	fea::mesh::elements::Mechanic::inelastic(true);
	model.mesh()->add_element(fea::mesh::elements::type::beam2, {0, 1});

	//supports
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_y);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::rotation_z);

	//loads
	model.boundary()->add_load_case();
	model.boundary()->load_case(0)->add_load_node(1, fea::mesh::nodes::dof::translation_y, -500e3);
	model.boundary()->load_case(0)->add_load_node(1, fea::mesh::nodes::dof::rotation_z, 7.5e3);

	//solver
	model.analysis()->solver(fea::analysis::solvers::type::static_nonlinear);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->step_max(300);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_max(2.0);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_predictor(0.01);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->strategy(fea::analysis::strategies::type::arc_length_cylindric);

	//solve
	model.analysis()->solve();
	
	//save
	model.save();
}
