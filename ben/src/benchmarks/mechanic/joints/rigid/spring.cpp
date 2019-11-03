//std
#include <cmath>
#include <ctime>

//fea
#include "Model/Model.h"

#include "Mesh/Mesh.h"
#include "Mesh/Nodes/Dofs.h"
#include "Mesh/Cells/Types.h"
#include "Mesh/Joints/Types.h"
#include "Mesh/Joints/Joint.h"
#include "Mesh/Sections/Types.h"
#include "Mesh/Elements/Types.h"
#include "Mesh/Materials/Types.h"
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
#include "benchmarks/mechanic/joints.h"

void tests::joint::rigid::spring(void)
{
	//parameters
	const double m = 1e3;
	const double k = 1e3;
	const unsigned n = 8;
	
	//model
	fea::models::Model model("spring", "benchmarks/joints/rigid");

	//nodes
	model.mesh()->add_node(+0, +0, +0);
	model.mesh()->add_node(+0, +0, -1);
	model.mesh()->add_node(+0, +0, +1);
	for(unsigned i = 0; i < n; i++)
	{
		const double t = 2 * M_PI * i / n;
		model.mesh()->add_node(cos(t), sin(t), +0);
		model.mesh()->add_node(cos(t), sin(t), -1);
		model.mesh()->add_node(cos(t), sin(t), +1);
	}

	//cells
	model.mesh()->add_cell(fea::mesh::cells::type::beam);

	//materials
	model.mesh()->add_material(fea::mesh::materials::type::steel);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->elastic_modulus(1e3);

	//sections
	model.mesh()->add_section(fea::mesh::sections::type::rectangle);

	//elements
	fea::mesh::elements::Mechanic::geometric(true);
	for(unsigned i = 0; i < n; i++)
	{
		const double t = 2 * M_PI * i / n;
		model.mesh()->add_element(fea::mesh::elements::type::beam3, {0, 3 * (i + 1) + 1});
		model.mesh()->add_element(fea::mesh::elements::type::beam3, {0, 3 * (i + 1) + 2});
		model.mesh()->add_element(fea::mesh::elements::type::beam3, {1, 3 * (i + 1) + 1});
		model.mesh()->add_element(fea::mesh::elements::type::beam3, {2, 3 * (i + 1) + 2});
		model.mesh()->add_element(fea::mesh::elements::type::beam3, {3 * (i + 1) + 0, 3 * (i + 1) + 1});
		model.mesh()->add_element(fea::mesh::elements::type::beam3, {3 * (i + 1) + 0, 3 * (i + 1) + 2});
		((fea::mesh::elements::Beam3*) model.mesh()->element(6 * i + 4))->orientation(cos(t), sin(t), 0);
		((fea::mesh::elements::Beam3*) model.mesh()->element(6 * i + 5))->orientation(cos(t), sin(t), 0);
	}
	
	//joints
	model.mesh()->add_joint(fea::mesh::joints::type::rigid, { 0 });
	for(unsigned i = 0; i < n; i++)
	{
		model.mesh()->joint(0)->add_node(3 * (i + 1) + 0);
	}

	//supports
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_y);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_z);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::rotation_x, 0, 0, k);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::rotation_y, 0, 0, k);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::rotation_z, 0, 0, k);

	//direction
	srand(time(nullptr));
    const double at = 2 * M_PI * (double) rand() / RAND_MAX;
    const double bt = 2 * M_PI * (double) rand() / RAND_MAX;
    
    //loads
    model.boundary()->add_load_case();
    model.boundary()->load_case(0)->add_load_node(0, fea::mesh::nodes::dof::rotation_z, k * M_PI * sin(bt));
    model.boundary()->load_case(0)->add_load_node(0, fea::mesh::nodes::dof::rotation_x, k * M_PI * cos(at) * cos(bt));
    model.boundary()->load_case(0)->add_load_node(0, fea::mesh::nodes::dof::rotation_y, k * M_PI * sin(at) * cos(bt));

	//solver
	model.analysis()->solver(fea::analysis::solvers::type::static_nonlinear);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->step_max(98);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_max(1e2);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_predictor(1e-2);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->watch_dof(0, fea::mesh::nodes::dof::rotation_z);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->strategy(fea::analysis::strategies::type::minimal_norm);

	//solve
	model.analysis()->solve();
	
	//save
	model.save();
}
