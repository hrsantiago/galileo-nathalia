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
#include "Mesh/Sections/Rectangle.h"
#include "Mesh/Elements/Mechanic/Mechanic.h"
#include "Mesh/Elements/Mechanic/Frame/Beam3.h"
#include "Mesh/Materials/Mechanic/Associative/Steel.h"

#include "Boundary/Boundary.h"
#include "Boundary/Cases/Load_Case.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Types.h"
#include "Analysis/Strategies/Types.h"
#include "Analysis/Solvers/Static_Nonlinear.h"

//ben
#include "benchmarks/mechanic/beam.h"

void tests::beam::static_nonlinear::elastic::self_weight(void)
{
	//model
	fea::models::Model model("self weight", "benchmarks/beam/static/nonlinear/elastic");

	//parameters
	const double l = 1.00e+00;
	const double h = 2.00e-01;
	const double t = 1.00e-02;
	const double E = 2.00e+11;
	const double r = 7.85e+03;
	const double g = 9.81e+00;
	
	const unsigned n = 10;

	const double dl = l / n;

	//nodes
	model.mesh()->add_node(0, 0, 0);
	model.mesh()->add_node(l, 0, 0);

	//cells
	model.mesh()->add_cell(fea::mesh::cells::type::beam);

	//materials
	model.mesh()->add_material(fea::mesh::materials::type::steel);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->elastic_modulus(E);

	//sections
	model.mesh()->add_section(fea::mesh::sections::type::rectangle);
	((fea::mesh::sections::Rectangle*) model.mesh()->section(0))->width(t);
	((fea::mesh::sections::Rectangle*) model.mesh()->section(0))->height(h);

	//elements
	fea::mesh::elements::Mechanic::geometric(true);
	model.mesh()->add_element(fea::mesh::elements::type::beam3, {0, 1});
	((fea::mesh::elements::Beam3*) model.mesh()->element(0))->orientation(0, 1, 0);

	//refine
	fea::mesh::cells::Line::refine(0, n);

	//supports
	model.boundary()->add_support(0, fea::mesh::nodes::dof::rotation_x);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_y);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_z);
	model.boundary()->add_support(1, fea::mesh::nodes::dof::translation_y);
	model.boundary()->add_support(1, fea::mesh::nodes::dof::translation_z);

	//loads
	model.boundary()->add_load_case("gravity");
	for(unsigned i = 0; i < n; i++)
	{
		const unsigned ni = model.mesh()->element(i)->index_node(0);
		const unsigned nj = model.mesh()->element(i)->index_node(1);
		model.boundary()->load_case(0)->add_load_node(ni, fea::mesh::nodes::dof::translation_y, -r * h * t * dl * g / 2);
		model.boundary()->load_case(0)->add_load_node(nj, fea::mesh::nodes::dof::translation_y, -r * h * t * dl * g / 2);
	}
	model.boundary()->load_case(0)->add_load_node(0, fea::mesh::nodes::dof::rotation_z, -r * h * t * g * dl * dl / 12);
	model.boundary()->load_case(0)->add_load_node(1, fea::mesh::nodes::dof::rotation_z, +r * h * t * g * dl * dl / 12);

	//solver
	model.analysis()->solver(fea::analysis::solvers::type::static_nonlinear);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*>(model.analysis()->solver())->step_max(200);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*>(model.analysis()->solver())->load_max(1.0);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*>(model.analysis()->solver())->load_predictor(1e-2);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*>(model.analysis()->solver())->strategy(fea::analysis::strategies::type::control_load);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*>(model.analysis()->solver())->watch_dof(n / 2 + 1, fea::mesh::nodes::dof::translation_y);

	//solve
	model.analysis()->solve();
	
	//save
	model.save();
}
