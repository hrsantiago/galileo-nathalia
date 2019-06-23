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

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Types.h"
#include "Analysis/Strategies/Types.h"
#include "Analysis/Solvers/Static_Nonlinear.h"

//ben
#include "benchmarks/mechanic/beam.h"

void tests::beam::static_nonlinear::elastic::hockling_cable(void) 
{
	/*
	Hockling cable
	Literature: Phd Thesis V. L. Corvec (2012) pp. 121
	*/

	//model
	fea::models::Model model("hockling cable", "benchmarks/beam/static/nonlinear/elastic");

	//nodes
	model.mesh()->add_node(0.00, 0.00, 0.00);
	model.mesh()->add_node(0.24, 1e-3, 0.00);

	//cells
	model.mesh()->add_cell(fea::mesh::cells::type::beam);

	//materials
	model.mesh()->add_material(fea::mesh::materials::type::steel);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->poisson_ratio(0.31);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->elastic_modulus(71.24e9);

	//sections
	model.mesh()->add_section(fea::mesh::sections::type::generic);
	((fea::mesh::sections::Generic*) model.mesh()->section(0))->area(1.00e-6);
	((fea::mesh::sections::Generic*) model.mesh()->section(0))->inercia_x(2.16e-12);
	((fea::mesh::sections::Generic*) model.mesh()->section(0))->inercia_y(8.33e-14);
	((fea::mesh::sections::Generic*) model.mesh()->section(0))->inercia_z(8.33e-14);

	//elements
	fea::mesh::elements::Mechanic::geometric(true);
	model.mesh()->add_element(fea::mesh::elements::type::beam3, {0, 1});

	//refine
	fea::mesh::cells::Line::refine(0, 20);

	//supports
	model.boundary()->add_support(0, fea::mesh::nodes::dof::rotation_x);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::rotation_y);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::rotation_z);
	model.boundary()->add_support(1, fea::mesh::nodes::dof::rotation_y);
	model.boundary()->add_support(1, fea::mesh::nodes::dof::rotation_z);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_y);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_z);
	model.boundary()->add_support(1, fea::mesh::nodes::dof::translation_y);
	model.boundary()->add_support(1, fea::mesh::nodes::dof::translation_z);

	//loads
	model.boundary()->add_load_case(1, fea::mesh::nodes::dof::rotation_x, 250e-3);

	//solver
	model.analysis()->solver(fea::analysis::solvers::type::static_nonlinear);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*>(model.analysis()->solver())->step_max(500);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*>(model.analysis()->solver())->load_max(1.0);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*>(model.analysis()->solver())->iteration_max(15);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*>(model.analysis()->solver())->load_adjust(true);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*>(model.analysis()->solver())->load_predictor(0.01);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*>(model.analysis()->solver())->iteration_desired(7);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*>(model.analysis()->solver())->watch_dof(1, fea::mesh::nodes::dof::rotation_x);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*>(model.analysis()->solver())->strategy(fea::analysis::strategies::type::minimal_norm);

	//solve
	model.analysis()->solve();
	
	//save
	model.save();
}
