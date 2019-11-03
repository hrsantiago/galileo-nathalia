//std
#include <cmath>

//fea
#include "Model/Model.h"

#include "Mesh/Mesh.h"
#include "Mesh/Nodes/Dofs.h"
#include "Mesh/Cells/Types.h"
#include "Mesh/Sections/Ring.h"
#include "Mesh/Sections/Types.h"
#include "Mesh/Elements/Types.h"
#include "Mesh/Materials/Types.h"
#include "Mesh/Elements/Mechanic/Mechanic.h"
#include "Mesh/Elements/Mechanic/Frame/Bar.h"
#include "Mesh/Materials/Mechanic/Associative/Steel.h"

#include "Boundary/Boundary.h"
#include "Boundary/Dependencies/Dependency.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Types.h"
#include "Analysis/Strategies/Types.h"
#include "Analysis/Solvers/Static_Nonlinear.h"

//ben
#include "benchmarks/mechanic/bar.h"

void tests::bar::static_nonlinear::tent_unit(void)
{
	//parameters
	const bool base = false;
	const bool strain = false;
	const double d = 5.08e-01;
	const double t = 12.7e-03;
	const double b = 7.00e+00;
	const double a = 1.20e+00;
	const double k = 0.00e+00;
	const double g = 0.15e+00;
	const double s = 4.00e+08;
	const double E = 2.00e+11;
	const double K = g / (1 - g) * E;

	const unsigned n = 6;

	const double h = b / a;
	const double q = 0 * h;
	const double w = d - 2 * t;
	const double l = sqrt(b * b + h * h);
	const double A = M_PI * (d * d - w * w) / 4;

	const double P = n * E * A * pow(h / l, 3) / 2;

	const fea::mesh::nodes::dof ux = fea::mesh::nodes::dof::translation_x;
	const fea::mesh::nodes::dof uy = fea::mesh::nodes::dof::translation_y;
	const fea::mesh::nodes::dof uz = fea::mesh::nodes::dof::translation_z;

	//model
	fea::models::Model model("tent unit", "benchmarks/bar/static/nonlinear");

	//nodes
	model.mesh()->add_node(q, 0, h);
	for(unsigned i = 0; i < n; i++)
	{
		const double t = 2 * i * M_PI / n;
		model.mesh()->add_node(b * cos(t), b * sin(t), 0);
	}

	//cells
	model.mesh()->add_cell(fea::mesh::cells::type::bar);

	//materials
	model.mesh()->add_material(fea::mesh::materials::type::steel);
	((fea::mesh::materials::Steel*)model.mesh()->material(0))->yield_stress(s);
	((fea::mesh::materials::Steel*)model.mesh()->material(0))->break_strain(1e30);
	((fea::mesh::materials::Steel*)model.mesh()->material(0))->elastic_modulus(E);
	((fea::mesh::materials::Steel*)model.mesh()->material(0))->plastic_modulus(K);

	//sections
	model.mesh()->add_section(fea::mesh::sections::type::ring);
	((fea::mesh::sections::Ring*)model.mesh()->section(0))->diameter(d);
	((fea::mesh::sections::Ring*)model.mesh()->section(0))->thickness(t);

	//elements
	for(unsigned i = 0; i < n; i++)
	{
		model.mesh()->add_element(fea::mesh::elements::type::bar, {0, i + 1});
	}

	//supports
	for(unsigned i = 0; i < n; i++)
	{
		model.boundary()->add_support(i + 1, uz);
		if(!base)
		{
			model.boundary()->add_support(i + 1, ux);
			model.boundary()->add_support(i + 1, uy);
		}
		else
		{
			model.boundary()->add_support(i + 1, ux, k * E * A / h);
			model.boundary()->add_support(i + 1, uy, k * E * A / h);
		}
	}

	//dependencies
	if(base)
	{
		for(unsigned i = 1; i < n; i++)
		{
			const double t = 2 * i * M_PI / n;
			model.boundary()->add_dependency(i + 1, ux, { 1 }, { ux });
			model.boundary()->add_dependency(i + 1, uy, { 1 }, { ux });
			model.boundary()->dependency(2 * (i - 1) + 0)->state([t](double* d) { return cos(t) * d[0]; });
			model.boundary()->dependency(2 * (i - 1) + 1)->state([t](double* d) { return sin(t) * d[0]; });
			model.boundary()->dependency(2 * (i - 1) + 0)->gradient([t](double*, unsigned) { return cos(t); });
			model.boundary()->dependency(2 * (i - 1) + 1)->gradient([t](double*, unsigned) { return sin(t); });
		}
	}

	//intials
//	model.boundary()->add_initial(0, ux, 1e-3 * h, 0);

	//loads
	model.boundary()->add_load_case(0, uz, -P);

	//solver
	fea::mesh::elements::Bar::strain(strain);
	fea::mesh::elements::Mechanic::geometric(true);
	fea::mesh::elements::Mechanic::inelastic(false);
	model.analysis()->solver(fea::analysis::solvers::type::static_nonlinear);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*>(model.analysis()->solver())->load_max(100);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*>(model.analysis()->solver())->step_max(2000);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*>(model.analysis()->solver())->dof_min(-2 * h);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*>(model.analysis()->solver())->watch_dof(0, uz);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*>(model.analysis()->solver())->frequencies(true);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*>(model.analysis()->solver())->load_predictor(0.01);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*>(model.analysis()->solver())->strategy(fea::analysis::strategies::type::control_state);

	//solve
	model.analysis()->solve();

	//save
	model.save();
}
