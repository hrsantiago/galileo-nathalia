//std
#include <cmath>

//fea
#include "Model/Model.h"

#include "Mesh/Mesh.h"
#include "Mesh/Nodes/Dofs.h"
#include "Mesh/Cells/Types.h"
#include "Mesh/Joints/Types.h"
#include "Mesh/Joints/Hinge.h"
#include "Mesh/Sections/Types.h"
#include "Mesh/Elements/Types.h"
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
#include "benchmarks/mechanic/deployable.h"

void tests::deployable::static_nonlinear::slut_mult(void)
{
	//model
	fea::models::Model model("slut mult", "benchmarks/deployable/static/nonlinear");

	//parameters
	const double l = 1.00e+00;
	const double h = 4.00e-02;
	const double t = 1.00e-02;
	const double s = 1.00e-02;
	const double e = 5.00e-02;
	const double k = 1.00e+00;
	const double a = M_PI / 2;
	const double d = (t + s) / 2;
	
	const unsigned n = 10;

	//nodes
	model.mesh()->add_node(0, 0, 0 * l * sin(a));
	model.mesh()->add_node(0, 0, 2 * l * sin(a));
	for(unsigned i = 0; i < n; i++)
	{
		const double q = 2 * i * M_PI / n;
		model.mesh()->add_node(e * cos(q), e * sin(q), 0 * l * sin(a));
		model.mesh()->add_node(e * cos(q), e * sin(q), 2 * l * sin(a));
		model.mesh()->add_node((e + 0 * l * cos(a)) * cos(q) + d * sin(q), (e + 0 * l * cos(a)) * sin(q) - d * cos(q), 0 * l * sin(a));
		model.mesh()->add_node((e + 1 * l * cos(a)) * cos(q) + d * sin(q), (e + 1 * l * cos(a)) * sin(q) - d * cos(q), 1 * l * sin(a));
		model.mesh()->add_node((e + 2 * l * cos(a)) * cos(q) + d * sin(q), (e + 2 * l * cos(a)) * sin(q) - d * cos(q), 2 * l * sin(a));
		model.mesh()->add_node((e + 2 * l * cos(a)) * cos(q) - d * sin(q), (e + 2 * l * cos(a)) * sin(q) + d * cos(q), 0 * l * sin(a));
		model.mesh()->add_node((e + 1 * l * cos(a)) * cos(q) - d * sin(q), (e + 1 * l * cos(a)) * sin(q) + d * cos(q), 1 * l * sin(a));
		model.mesh()->add_node((e + 0 * l * cos(a)) * cos(q) - d * sin(q), (e + 0 * l * cos(a)) * sin(q) + d * cos(q), 2 * l * sin(a));
	}

	//cells
	model.mesh()->add_cell(fea::mesh::cells::type::beam);

	//materials
	model.mesh()->add_material(fea::mesh::materials::type::steel);

	//sections
	model.mesh()->add_section(fea::mesh::sections::type::rectangle);
	((fea::mesh::sections::Rectangle*) model.mesh()->section(0))->width(t);
	((fea::mesh::sections::Rectangle*) model.mesh()->section(0))->height(h);

	//elements
	for(unsigned i = 0; i < n; i++)
	{
		const double q = 2 * i * M_PI / n;
		model.mesh()->add_element(fea::mesh::elements::type::beam3, {8 * i + 2, 0});
		model.mesh()->add_element(fea::mesh::elements::type::beam3, {8 * i + 3, 1});
		model.mesh()->add_element(fea::mesh::elements::type::beam3, {8 * i + 4, 8 * i + 5});
		model.mesh()->add_element(fea::mesh::elements::type::beam3, {8 * i + 5, 8 * i + 6});
		model.mesh()->add_element(fea::mesh::elements::type::beam3, {8 * i + 7, 8 * i + 8});
		model.mesh()->add_element(fea::mesh::elements::type::beam3, {8 * i + 8, 8 * i + 9});
		((fea::mesh::elements::Beam3*) model.mesh()->element(6 * i + 0))->orientation(+sin(q), -cos(q), 0);
		((fea::mesh::elements::Beam3*) model.mesh()->element(6 * i + 1))->orientation(+sin(q), -cos(q), 0);
		((fea::mesh::elements::Beam3*) model.mesh()->element(6 * i + 2))->orientation(+sin(q), -cos(q), 0);
		((fea::mesh::elements::Beam3*) model.mesh()->element(6 * i + 3))->orientation(+sin(q), -cos(q), 0);
		((fea::mesh::elements::Beam3*) model.mesh()->element(6 * i + 4))->orientation(+sin(q), -cos(q), 0);
		((fea::mesh::elements::Beam3*) model.mesh()->element(6 * i + 5))->orientation(+sin(q), -cos(q), 0);
	}

	//joints
	for(unsigned i = 0; i < n; i++)
	{
		const double q = 2 * i * M_PI / n;
		model.mesh()->add_joint(fea::mesh::joints::type::hinge, {8 * i + 2, 8 * i + 4});
		model.mesh()->add_joint(fea::mesh::joints::type::hinge, {8 * i + 3, 8 * i + 9});
		model.mesh()->add_joint(fea::mesh::joints::type::hinge, {8 * i + 5, 8 * i + 8});
		((fea::mesh::joints::Hinge*) model.mesh()->joint(3 * i + 2))->stiffness(0, 2, k);
		for(unsigned j = 0; j < 3; j++)
		{
			((fea::mesh::joints::Hinge*) model.mesh()->joint(3 * i + j))->axis(+sin(q), -cos(q), 0);
			((fea::mesh::joints::Hinge*) model.mesh()->joint(3 * i + j))->orientation(+cos(q), +sin(q), 0);
		}
	}

	//supports
	model.boundary()->add_support(0, fea::mesh::nodes::dof::rotation_x);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::rotation_y);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::rotation_z);
	model.boundary()->add_support(1, fea::mesh::nodes::dof::rotation_x);
	model.boundary()->add_support(1, fea::mesh::nodes::dof::rotation_y);
	model.boundary()->add_support(1, fea::mesh::nodes::dof::rotation_z);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_y);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_z);
	model.boundary()->add_support(1, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(1, fea::mesh::nodes::dof::translation_y);

	//loads
	model.boundary()->add_load_case();
	for(unsigned i = 0; i < n; i++)
	{
		const double q = 2 * i * M_PI / n;
		model.boundary()->load_case(0)->add_load_node(8 * i + 7, fea::mesh::nodes::dof::translation_x, k / l * cos(q));
		model.boundary()->load_case(0)->add_load_node(8 * i + 7, fea::mesh::nodes::dof::translation_y, k / l * sin(q));
	}

	//solver
	fea::mesh::elements::Mechanic::geometric(true);
	model.analysis()->solver(fea::analysis::solvers::type::static_nonlinear);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->step_max(700);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_max(7e1);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->dof_max(M_PI / 2);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_adjust(true);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_predictor(1e-2);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->iteration_desired(4);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->watch_dof(4, fea::mesh::nodes::dof::rotation_y);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->strategy(fea::analysis::strategies::type::minimal_norm);

	//solve
	model.analysis()->solve();
	
	//save
	model.save();
}
