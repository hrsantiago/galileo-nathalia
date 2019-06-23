//std
#include <cmath>

//mat
#include "misc/defs.h"

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

#include "Boundary/Boundary.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Types.h"
#include "Analysis/Strategies/Types.h"
#include "Analysis/Solvers/Static_Nonlinear.h"

//ben
#include "benchmarks/mechanic/beam.h"

void tests::beam::static_nonlinear::elastic::arc_cantilever(void) 
{
	/*
	Arc cantilever bend subjected to tip force
	Literature: Phd Thesis P. Carotenuto (2006) pp. 95
	 */

	//model
	fea::models::Model model("arc cantilever", "benchmarks/beam/static/nonlinear/elastic");

	//nodes
	const double r = 10;
	const unsigned ne = 8;
	const unsigned nn = ne + 1;
	for(unsigned i = 0; i < nn; i++)
	{
		const double t = M_PI * i / 4 / ne;
		model.mesh()->add_node(r * cos(t), r * sin(t), 0);
	}

	//cells
	model.mesh()->add_cell(fea::mesh::cells::type::beam);

	//materials
	model.mesh()->add_material(fea::mesh::materials::type::steel);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->poisson_ratio(0.0);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->elastic_modulus(1.0e7);

	//sections
	model.mesh()->add_section(fea::mesh::sections::type::rectangle);
	((fea::mesh::sections::Rectangle*) model.mesh()->section(0))->width(0.10);
	((fea::mesh::sections::Rectangle*) model.mesh()->section(0))->height(0.10);

	//elements
	fea::mesh::elements::Mechanic::geometric(true);
	for(unsigned i = 0; i < ne; i++)
	{
		model.mesh()->add_element(fea::mesh::elements::type::beam3, {i, i + 1});
	}

	//supports
	model.boundary()->add_support(0, fea::mesh::nodes::dof::rotation_x);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::rotation_y);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::rotation_z);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_y);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_z);

	//loads
	model.boundary()->add_load_case(ne, fea::mesh::nodes::dof::translation_z, 50);

	//solver
	model.analysis()->solver(fea::analysis::solvers::type::static_nonlinear);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->step_max(60);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_max(1.0);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_predictor(0.0025);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->watch_dof(nn - 1, fea::mesh::nodes::dof::translation_z);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->strategy(fea::analysis::strategies::type::arc_length_cylindric);

	//solve
	model.analysis()->solve();
	
	//save
	model.save();
}
