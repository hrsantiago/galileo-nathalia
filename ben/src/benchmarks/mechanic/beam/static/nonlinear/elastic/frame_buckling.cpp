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

void tests::beam::static_nonlinear::elastic::frame_buckling(void) 
{
	/* Frame buckling */

	//parameters
	const double l = 5.00e+00;
	const double E = 2.00e+08;
	const double A0 = 1.74e-02;
	const double A1 = 2.76e-02;
	const double I0 = 5.24e-04;
	const double I1 = 1.16e-03;

	//model
	fea::models::Model model("frame buckling", "benchmarks/beam/static/nonlinear/elastic");

	//nodes
	model.mesh()->add_node(0, 0, 0);
	model.mesh()->add_node(0, l, 0);
	model.mesh()->add_node(2 * l, l, 0);
	model.mesh()->add_node(2 * l, 0, 0);

	//cells
	model.mesh()->add_cell(fea::mesh::cells::type::beam);
	model.mesh()->add_cell(fea::mesh::cells::type::beam);
	((fea::mesh::cells::Line*) model.mesh()->cell(0))->section(0);
	((fea::mesh::cells::Line*) model.mesh()->cell(1))->section(1);
	
	//materials
	model.mesh()->add_material(fea::mesh::materials::type::steel);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->elastic_modulus(E);

	//sections
	model.mesh()->add_section(fea::mesh::sections::type::generic);
	model.mesh()->add_section(fea::mesh::sections::type::generic);
	((fea::mesh::sections::Generic*) model.mesh()->section(0))->area(A0);
	((fea::mesh::sections::Generic*) model.mesh()->section(1))->area(A1);
	((fea::mesh::sections::Generic*) model.mesh()->section(0))->inercia_z(I0);
	((fea::mesh::sections::Generic*) model.mesh()->section(1))->inercia_z(I1);

	//elements
	fea::mesh::elements::Mechanic::geometric(true);
	model.mesh()->add_element(fea::mesh::elements::type::beam2, {0, 1}, 0, 0);
	model.mesh()->add_element(fea::mesh::elements::type::beam2, {1, 2}, 0, 1);
	model.mesh()->add_element(fea::mesh::elements::type::beam2, {2, 3}, 0, 0);

	//refine
	fea::mesh::cells::Line::refine(0, 1);
	fea::mesh::cells::Line::refine(1, 1);
	fea::mesh::cells::Line::refine(2, 1);

	//supports
	model.boundary()->add_support(0, fea::mesh::nodes::dof::rotation_z);
	model.boundary()->add_support(3, fea::mesh::nodes::dof::rotation_z);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_y);
	model.boundary()->add_support(3, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(3, fea::mesh::nodes::dof::translation_y);

	//loads
	model.boundary()->add_load_case();
	model.boundary()->load_case(0)->add_load_node(1, fea::mesh::nodes::dof::translation_x, +3e3);
	model.boundary()->load_case(0)->add_load_node(1, fea::mesh::nodes::dof::translation_y, -3e4);
	model.boundary()->load_case(0)->add_load_node(2, fea::mesh::nodes::dof::translation_y, -3e4);

	//solver
	model.analysis()->solver(fea::analysis::solvers::type::static_nonlinear);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->step_max(900);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_max(1.0);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_predictor(0.01);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->watch_dof(1, fea::mesh::nodes::dof::translation_x);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->strategy(fea::analysis::strategies::type::control_load);

	//solve
	model.analysis()->solve();
	
	//save
	model.save();
}
