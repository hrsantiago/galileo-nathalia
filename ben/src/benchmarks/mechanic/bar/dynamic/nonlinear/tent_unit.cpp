//std
#include <cmath>

//mat
#include "misc/defs.h"

//fea
#include "Models/Model.h"

#include "Mesh/Mesh.h"
#include "Mesh/Nodes/Dofs.h"
#include "Mesh/Cells/Types.h"
#include "Mesh/Sections/Ring.h"
#include "Mesh/Sections/Types.h"
#include "Mesh/Elements/Types.h"
#include "Mesh/Materials/Types.h"
#include "Mesh/Elements/Mechanic/Mechanic.h"
#include "Mesh/Materials/Mechanic/Associative/Steel.h"

#include "Boundary/Boundary.h"
#include "Boundary/Cases/Load_Case.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Types.h"
#include "Analysis/Solvers/Dynamic_Nonlinear.h"

//ben
#include "benchmarks/mechanic/bar.h"

void tests::bar::dynamic_nonlinear::tent_unit(void) 
{
	//model
	fea::models::Model model("tent unit", "benchmarks/bar/dynamic/nonlinear");

	//parameters
	const unsigned nb = 6;
	const unsigned np = 10;
	const unsigned nt = 100;
	const unsigned ns = nt * np;
	
	const double d = 5.08e-1;
	const double t = 12.7e-3;
	const double l = 6.00e+0;
	const double E = 2.00e11;
	const double r = 7.85e+3;
	const double a = 60 * M_PI / 180;
	
	const double b = l * cos(a);
	const double h = l * sin(a);
	
	const double A = M_PI * (pow(d, 2) - pow(d - 2 * t, 2)) / 4;
	
	const double w = sqrt(3 * E / r) * h / (l * l);
	const double P = nb * E * A * pow(h / l, 3) / 2;
	const double c = 2 * nb * A * h / l * sqrt(E * r / 3);
	
	const double ul = sqrt(3) * h / 3 - h;
	const double ub = sqrt(h * h - b * b) - h;
	
	const double Pl = 2 * sqrt(3) / 9 * P;
	
	//nodes
	model.mesh()->add_node(0, 0, h);
	for(unsigned i = 0; i < nb; i++) 
	{
		const double t = 2 * M_PI * i / nb;
		model.mesh()->add_node(b * cos(t), b * sin(t), 0);
	}

	//cells
	model.mesh()->add_cell(fea::mesh::cells::type::bar);

	//materials
	model.mesh()->add_material(fea::mesh::materials::type::steel);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->specific_mass(r);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->elastic_modulus(E);

	//sections
	model.mesh()->add_section(fea::mesh::sections::type::ring);
	((fea::mesh::sections::Ring*) model.mesh()->section(0))->diameter(d);
	((fea::mesh::sections::Ring*) model.mesh()->section(0))->thickness(t);

	//elements
	fea::mesh::elements::Mechanic::geometric(true);
	for(unsigned i = 0; i < nb; i++) 
	{
		model.mesh()->add_element(fea::mesh::elements::type::bar, {0, i + 1});
	}

	//supports
	for(unsigned i = 1; i < nb + 1; i++) 
	{
		model.boundary()->add_support(i, fea::mesh::nodes::dof::translation_x);
		model.boundary()->add_support(i, fea::mesh::nodes::dof::translation_y);
		model.boundary()->add_support(i, fea::mesh::nodes::dof::translation_z);
	}
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_x, 0, 4.00e-2 * c, 0);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_y, 0, 4.00e-2 * c, 0);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_z, 0, 4.00e-2 * c, 0);

	//initials
	model.boundary()->add_initial(0, fea::mesh::nodes::dof::translation_z, 0, 0);

	//loads
	model.boundary()->add_load_case(0, fea::mesh::nodes::dof::translation_z, Pl, [w](double t) { return sin(w * t); });

	//solver
	model.analysis()->solver(fea::analysis::solvers::type::dynamic_nonlinear);
	dynamic_cast<fea::analysis::solvers::Dynamic_Nonlinear*> (model.analysis()->solver())->step_max(ns);
	dynamic_cast<fea::analysis::solvers::Dynamic_Nonlinear*> (model.analysis()->solver())->time_max(2 * np * M_PI / w);
	dynamic_cast<fea::analysis::solvers::Dynamic_Nonlinear*> (model.analysis()->solver())->watch_dof(0, fea::mesh::nodes::dof::translation_z);
	dynamic_cast<fea::analysis::solvers::Dynamic_Nonlinear*> (model.analysis()->solver())->integration(fea::analysis::solvers::integration::newmark);
	dynamic_cast<fea::analysis::solvers::Dynamic_Nonlinear*> (model.analysis()->solver())->convergence((unsigned) fea::analysis::solvers::convergence::fixed);

	//solve
	model.analysis()->solve();
	
	//save
	model.save();
}
