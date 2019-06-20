//std
#include <cmath>

//fea
#include "Models/Model.h"

#include "Mesh/Mesh.h"
#include "Mesh/Nodes/Dofs.h"
#include "Mesh/Cells/Types.h"
#include "Mesh/Joints/Types.h"
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
#include "benchmarks/mechanic/beam.h"

void tests::beam::static_nonlinear::elastic::von_mises_buckling(void) 
{
	/*
	von mises truss buckling
	Literature: International Journal of nonlinear mechanics - F. Bazzucchi et al. (2017) pp. 11-20
	 */

	//model
	fea::models::Model model("von mises buckling", "benchmarks/beam/static/nonlinear/elastic");

	//parameters
	const double r = 5.0;
	const double h = 1.0;
	const double l = sqrt(r * r + h * h);

	//nodes
	model.mesh()->add_node(0, 0, 0);
	model.mesh()->add_node(r, h, 0);
	model.mesh()->add_node(r, h, 0);
	model.mesh()->add_node(2 * r, 0, 0);

	//cells
	model.mesh()->add_cell(fea::mesh::cells::type::beam);

	//sections
	model.mesh()->add_section(fea::mesh::sections::type::rectangle);
	((fea::mesh::sections::Rectangle*) model.mesh()->section(0))->width(0.1);
	((fea::mesh::sections::Rectangle*) model.mesh()->section(0))->height(0.1);

	//materials
	model.mesh()->add_material(fea::mesh::materials::type::steel);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->elastic_modulus(200e9);

	//elements
	model.mesh()->add_element(fea::mesh::elements::type::beam2, {0, 1});
	model.mesh()->add_element(fea::mesh::elements::type::beam2, {2, 3});

	//joints
	model.mesh()->add_joint(fea::mesh::joints::type::pinned, {1, 2});

	//supports
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_y);
	model.boundary()->add_support(3, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(3, fea::mesh::nodes::dof::translation_y);

	//loads
	model.boundary()->add_load_case();
	model.boundary()->load_case(0)->add_load_node(1, fea::mesh::nodes::dof::rotation_x, -1e2);
	model.boundary()->load_case(0)->add_load_node(1, fea::mesh::nodes::dof::rotation_y, -1e2);
	model.boundary()->load_case(0)->add_load_node(1, fea::mesh::nodes::dof::rotation_z, -1e2);
	model.boundary()->load_case(0)->add_load_node(1, fea::mesh::nodes::dof::translation_x, -1e2);
	model.boundary()->load_case(0)->add_load_node(1, fea::mesh::nodes::dof::translation_y, -1e2);
	model.boundary()->load_case(0)->add_load_node(1, fea::mesh::nodes::dof::translation_z, -1e2);

	//solver
	model.analysis()->solver(fea::analysis::solvers::type::static_nonlinear);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->step_max(200);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_max(10);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_predictor(0.2);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_increment_max(0.2);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->strategy(fea::analysis::strategies::type::minimal_norm);

	//solve
	model.analysis()->solve();
	
	//save
	model.save();
}
