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

void tests::beam::static_nonlinear::elastic::arc_circle(void) 
{
	/*
	Pinned circle arc subjected to a compression vertical load
	Literature: UFOP Phd Thesis A. S. Galvao (2000) pp. 157
	 */

	//model
	fea::models::Model model("arc circle", "benchmarks/beam/static/nonlinear/elastic");

	//nodes
	const double R = 50;
	const unsigned ne = 50;
	const unsigned nn = ne + 1;
	for(unsigned i = 0; i < nn; i++)
	{
		const double t = M_PI * i / (nn - 1);
		const double x = R * cos(t);
		const double y = R * sin(t);
		model.mesh()->add_node(x, y, 0);
	}

	//cells
	model.mesh()->add_cell(fea::mesh::cells::type::beam);

	//materials
	model.mesh()->add_material(fea::mesh::materials::type::steel);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->elastic_modulus(2000);

	//sections
	model.mesh()->add_section(fea::mesh::sections::type::generic);
	((fea::mesh::sections::Generic*) model.mesh()->section(0))->area(10.0);
	((fea::mesh::sections::Generic*) model.mesh()->section(0))->inercia_z(1.0);

	//elements
	fea::mesh::elements::Mechanic::geometric(true);
	for(unsigned i = 0; i < ne; i++)
	{
		model.mesh()->add_element(fea::mesh::elements::type::beam3, {i, i + 1});
	}

	//supports
	model.boundary()->add_support(0, fea::mesh::nodes::dof::rotation_x);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::rotation_y);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_y);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_z);
	model.boundary()->add_support(nn - 1, fea::mesh::nodes::dof::rotation_x);
	model.boundary()->add_support(nn - 1, fea::mesh::nodes::dof::rotation_y);
	model.boundary()->add_support(nn - 1, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(nn - 1, fea::mesh::nodes::dof::translation_y);
	model.boundary()->add_support(nn - 1, fea::mesh::nodes::dof::translation_z);

	//loads
	model.boundary()->add_load_case((nn - 1) / 2, fea::mesh::nodes::dof::translation_y, -1);

	//solver
	model.analysis()->solver(fea::analysis::solvers::type::static_nonlinear);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->step_max(800);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_max(800);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_adjust(true);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_predictor(1.0);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->iteration_desired(3);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->strategy(fea::analysis::strategies::type::minimal_norm);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->watch_dof((nn - 1) / 2, fea::mesh::nodes::dof::translation_y);

	//solve
	model.analysis()->solve();
	
	//save
	model.save();
}
