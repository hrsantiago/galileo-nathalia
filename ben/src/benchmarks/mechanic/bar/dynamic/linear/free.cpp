//std
#include <cmath>

//fea
#include "Model/Model.h"

#include "Mesh/Mesh.h"
#include "Mesh/Nodes/Dofs.h"
#include "Mesh/Cells/Types.h"
#include "Mesh/Sections/Types.h"
#include "Mesh/Elements/Types.h"
#include "Mesh/Materials/Types.h"
#include "Mesh/Sections/Rectangle.h"
#include "Mesh/Materials/Mechanic/Associative/Steel.h"

#include "Boundary/Boundary.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Types.h"
#include "Analysis/Solvers/Dynamic_Linear.h"

//ben
#include "benchmarks/mechanic/bar.h"

void tests::bar::dynamic_linear::free(void) 
{
	/*
	Free vibration
	Literature: A. W. Leissa, M. S. Qatu - Vibrations of Continous Systems (2011) pp. 85
	 */

	//parameters
	const double d = 1.00e-1;
	const double l = 1.00e+0;
	const double u = 1.00e-1;
	const double r = 7.85e+3;
	const double E = 2.00e11;
	
	const double T = 2 * M_PI * l * sqrt(r / E / 3);
	
	const unsigned ne = 10;
	const unsigned nn = ne + 1;

	//model
	fea::models::Model model("free", "benchmarks/bar/dynamic/linear");

	//nodes
	for(unsigned i = 0; i < nn; i++) 
	{
		model.mesh()->add_node(i * l / ne, 0, 0);
	}

	//cells
	model.mesh()->add_cell(fea::mesh::cells::type::bar);

	//materials
	model.mesh()->add_material(fea::mesh::materials::type::steel);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->specific_mass(r);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->elastic_modulus(E);

	//sections
	model.mesh()->add_section(fea::mesh::sections::type::rectangle);
	((fea::mesh::sections::Rectangle*) model.mesh()->section(0))->width(d);
	((fea::mesh::sections::Rectangle*) model.mesh()->section(0))->height(d);

	//elements
	for(unsigned i = 0; i < ne; i++) 
	{
		model.mesh()->add_element(fea::mesh::elements::type::bar, {i, i + 1});
	}

	//supports
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_x);
	for(unsigned i = 0; i < nn; i++) 
	{
		model.boundary()->add_support(i, fea::mesh::nodes::dof::translation_y);
		model.boundary()->add_support(i, fea::mesh::nodes::dof::translation_z);
	}

	//initials
	for(unsigned i = 1; i < nn; i++)
	{
		model.boundary()->add_initial(i, fea::mesh::nodes::dof::translation_x, i * u / ne, 0);
	}

	//loads
	model.boundary()->add_load_case();

	//solver
	model.analysis()->solver(fea::analysis::solvers::type::dynamic_linear);
	dynamic_cast<fea::analysis::solvers::Dynamic_Linear*> (model.analysis()->solver())->step_max(800);
	dynamic_cast<fea::analysis::solvers::Dynamic_Linear*> (model.analysis()->solver())->time_max(5 * T);
	dynamic_cast<fea::analysis::solvers::Dynamic_Linear*> (model.analysis()->solver())->watch_dof(ne, fea::mesh::nodes::dof::translation_x);

	//solve
	model.analysis()->solve();
	
	//save
	model.save();
}
