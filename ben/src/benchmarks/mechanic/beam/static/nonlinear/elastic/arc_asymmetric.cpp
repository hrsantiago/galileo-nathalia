//std
#include <cmath>

//mat
#include "misc/defs.h"

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
#include "Mesh/Materials/Mechanic/Associative/Steel.h"

#include "Boundary/Boundary.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Types.h"
#include "Analysis/Strategies/Types.h"
#include "Analysis/Solvers/Static_Nonlinear.h"

//ben
#include "benchmarks/mechanic/beam.h"

void tests::beam::static_nonlinear::elastic::arc_asymmetric(void) 
{
	/*
	Asymmetric arc
	Literature: UFOP Phd Thesis A. S. Galvao (2000) pp. 149
	 */

	//model
	fea::models::Model model("arc asymmetric", "benchmarks/beam/static/nonlinear/elastic");

	//parameters
	const double R = 100;
	const double a = 35 * M_PI / 360;
	const double q = 215 * M_PI / 180;
	const unsigned ne = 64;
	const unsigned nn = ne + 1;

	//nodes
	for(unsigned i = 0; i < nn; i++)
	{
		const double t = q * i / (nn - 1) - a;
		const double x = R * cos(t);
		const double y = R * sin(t);
		model.mesh()->add_node(x, y, 0);
	}

	//cells
	model.mesh()->add_cell(fea::mesh::cells::type::beam);

	//materials
	model.mesh()->add_material(fea::mesh::materials::type::steel);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->elastic_modulus(12e6);

	//sections
	model.mesh()->add_section(fea::mesh::sections::type::rectangle);
	((fea::mesh::sections::Rectangle*) model.mesh()->section(0))->width(1.0);
	((fea::mesh::sections::Rectangle*) model.mesh()->section(0))->height(1.0);

	//elements
	fea::mesh::elements::Mechanic::geometric(true);
	for(unsigned i = 0; i < ne; i++)
	{
		model.mesh()->add_element(fea::mesh::elements::type::beam2, {i, i + 1});
	}

	//supports
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_y);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::rotation_z);
	model.boundary()->add_support(nn - 1, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(nn - 1, fea::mesh::nodes::dof::translation_y);

	//loads
	model.boundary()->add_load_case((nn - 1) / 2, fea::mesh::nodes::dof::translation_y, -1e2);

	//solver
	model.analysis()->solver(fea::analysis::solvers::type::static_nonlinear);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->step_max(500);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_max(10);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_predictor(0.1);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_adjust(true);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->iteration_desired(3);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->strategy(fea::analysis::strategies::type::minimal_norm);

	//solve
	model.analysis()->solve();
	
	//save
	model.save();
}
