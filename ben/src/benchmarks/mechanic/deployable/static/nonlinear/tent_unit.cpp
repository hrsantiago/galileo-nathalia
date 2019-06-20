//std
#include <cmath>

//mat
#include "misc/defs.h"

//fea
#include "Models/Model.h"

#include "Mesh/Mesh.h"
#include "Mesh/Nodes/Dofs.h"
#include "Mesh/Nodes/Node.h"
#include "Mesh/Cells/Types.h"
#include "Mesh/Joints/Types.h"
#include "Mesh/Joints/Hinge.h"
#include "Mesh/Sections/Ring.h"
#include "Mesh/Sections/Types.h"
#include "Mesh/Elements/Types.h"
#include "Mesh/Cells/Line/Line.h"
#include "Mesh/Materials/Types.h"
#include "Mesh/Elements/Mechanic/Mechanic.h"
#include "Mesh/Materials/Mechanic/Associative/Steel.h"

#include "Boundary/Boundary.h"
#include "Boundary/Cases/Load_Case.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Types.h"
#include "Analysis/Strategies/Types.h"
#include "Analysis/Solvers/Static_Nonlinear.h"

//ben
#include "benchmarks/mechanic/deployable.h"

void tests::deployable::static_nonlinear::tent_unit(void)
{
	//model
	fea::models::Model model("tent unit", "benchmarks/deployable/static/nonlinear");

	//parameters
	const double b = 7.00e+00;
	const double h = b / 7.00;
	const double d = 5.08e-01;
	const double t = 1.27e-02;
	const double E = 2.00e+11;
	const double s = 4.00e+08;
	const double k = 1.00e+05;
	
	const unsigned n = 6;
	const unsigned q = 5;
	
	const double l = sqrt(pow(h, 2) + pow(b, 2));
	const double A = M_PI * (pow(d, 2) - pow(d - 2 * t, 2)) / 4;
	const double I = M_PI * (pow(d, 4) - pow(d - 2 * t, 4)) / 64;
	
	const double P = n * E * A * pow(h / l, 3) / 2;

	//nodes
	model.mesh()->add_node(0, 0, h);
	for(unsigned i = 0; i < n; i++)
	{
		model.mesh()->add_node(0, 0, h);
		model.mesh()->add_node(b * cos(2 * M_PI * i / n), b * sin(2 * M_PI * i / n), 0);
	}

	//cells
	model.mesh()->add_cell(fea::mesh::cells::type::beam);

	//materials
	model.mesh()->add_material(fea::mesh::materials::type::steel);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->yield_stress(s);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->elastic_modulus(E);

	//sections
	model.mesh()->add_section(fea::mesh::sections::type::ring);
	((fea::mesh::sections::Ring*) model.mesh()->section(0))->mesh_r(5);
	((fea::mesh::sections::Ring*) model.mesh()->section(0))->mesh_t(30);
	((fea::mesh::sections::Ring*) model.mesh()->section(0))->diameter(d);
	((fea::mesh::sections::Ring*) model.mesh()->section(0))->thickness(t);

	//elements
	for(unsigned i = 0; i < n; i++)
	{
		model.mesh()->add_element(fea::mesh::elements::type::beam3, {2 * i + 1, 2 * i + 2});
	}

	//refine
	for(unsigned i = 0; i < n; i++)
	{
		fea::mesh::cells::Line::refine(i, q);
	}

	//imperfection
	for(unsigned i = 0, c = 2 * n; i < n; i++)
	{
		for(unsigned j = 1; j < q; j++)
		{
			const double t = 2 * M_PI * i / n;
			const double a = -1e-3 * l * sin(j * M_PI / q);
			const double* p = model.mesh()->node(++c)->coordinates();
			model.mesh()->node(c)->coordinates(p[0] + a * h * cos(t) / l, p[1] + a * h * sin(t) / l, p[2] + a * b / l);
		}
	}

	//joints
	for(unsigned i = 0; i < n; i++)
	{
		const double t = 2 * M_PI * i / n;
		model.mesh()->add_joint(fea::mesh::joints::type::hinge, {2 * i + 1, 0});
		((fea::mesh::joints::Hinge*) model.mesh()->joint(i))->stiffness(0, k, k, 0);
		((fea::mesh::joints::Hinge*) model.mesh()->joint(i))->axis(-sin(t), cos(t), 0);
		((fea::mesh::joints::Hinge*) model.mesh()->joint(i))->orientation(cos(t), sin(t), 0);
	}

	//supports
	for(unsigned i = 0; i < n; i++)
	{
		for(unsigned j = 0; j < 3; j++)
		{
			model.boundary()->add_support(2 * i + 2, fea::mesh::nodes::dof(1 << j));
		}
	}
	
	//loads
	const double m = 15 * M_PI / 180;
	model.boundary()->add_load_case(0, fea::mesh::nodes::dof::translation_x, cos(m) * P);
	model.boundary()->add_load_case(0, fea::mesh::nodes::dof::translation_y, sin(m) * P);

	//solver
	fea::mesh::elements::Mechanic::geometric(true);
	fea::mesh::elements::Mechanic::inelastic(false);
	model.analysis()->solver(fea::analysis::solvers::type::static_nonlinear);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->step_max(300);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_min(-5.0);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_max(+5.0);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->tolerance(1e-4);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->dof_min(-2.1 * h);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->dof_max(+2.1 * h);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_adjust(true);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_predictor(0.02);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->iteration_desired(1);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->watch_dof(0, fea::mesh::nodes::dof::translation_x);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->strategy(fea::analysis::strategies::type::control_state);

	//solve
	model.analysis()->solve();
	
	//save
	model.save();
}
