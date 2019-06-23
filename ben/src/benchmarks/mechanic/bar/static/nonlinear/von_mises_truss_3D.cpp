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
#include "benchmarks/mechanic/bar.h"

void tests::bar::static_nonlinear::von_mises_truss_3D(void) 
{
	//parameters
	const double d = 1.00e-01;
	const double E = 2.00e+11;
	const double l = 1.00e+00;
	const double k = 1.00e+09;
	const double r = 1.00e-03 * l;
	const double a = 60 * M_PI / 180;
	
	const double A = d * d;
	const double b = l * cos(a);
	const double h = l * sin(a);
	const double P = E * A * pow(h / l, 3);
	
	//model
	fea::models::Model model("von mises truss 3D", "benchmarks/bar/static/nonlinear");

	//nodes
	model.mesh()->add_node( 0, h, r);
	model.mesh()->add_node(-b, 0, 0);
	model.mesh()->add_node(+b, 0, 0);

	//materials
	model.mesh()->add_material(fea::mesh::materials::type::steel);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->elastic_modulus(E);

	//sections
	model.mesh()->add_section(fea::mesh::sections::type::rectangle);
	((fea::mesh::sections::Rectangle*) model.mesh()->section(0))->width(d);
	((fea::mesh::sections::Rectangle*) model.mesh()->section(0))->height(d);

	//cells
	model.mesh()->add_cell(fea::mesh::cells::type::bar);

	//elements
	fea::mesh::elements::Mechanic::geometric(true);
	model.mesh()->add_element(fea::mesh::elements::type::bar, {1, 0});
	model.mesh()->add_element(fea::mesh::elements::type::bar, {2, 0});

	//supports
	model.boundary()->add_support(1, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(1, fea::mesh::nodes::dof::translation_y);
	model.boundary()->add_support(1, fea::mesh::nodes::dof::translation_z);
	model.boundary()->add_support(2, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(2, fea::mesh::nodes::dof::translation_y);
	model.boundary()->add_support(2, fea::mesh::nodes::dof::translation_z);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_z, 0, 0, k);

	//loads
	model.boundary()->add_load_case(0, fea::mesh::nodes::dof::translation_y, -P);

	//solver
	model.analysis()->solver(fea::analysis::solvers::type::static_nonlinear);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->step_max(150);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_max(1e0);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_predictor(0.05);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_increment_max(1e30);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->watch_dof(0, fea::mesh::nodes::dof::translation_y);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->strategy(fea::analysis::strategies::type::arc_length_cylindric);

	//solve
	model.analysis()->solve();
	
	//save
	model.save();
}
