//std
#include <cmath>

//fea
#include "Model/Model.h"

#include "Mesh/Mesh.h"
#include "Mesh/Nodes/Dofs.h"
#include "Mesh/Cells/Types.h"
#include "Mesh/Joints/Types.h"
#include "Mesh/Joints/Joint.h"
#include "Mesh/Sections/Types.h"
#include "Mesh/Sections/Circle.h"
#include "Mesh/Sections/Ring.h"
#include "Mesh/Elements/Types.h"
#include "Mesh/Cells/Line/Line.h"
#include "Mesh/Cells/Line/Bar.h"
#include "Mesh/Materials/Types.h"
#include "Mesh/Elements/Mechanic/Mechanic.h"
#include "Mesh/Elements/Mechanic/Frame/Bar.h"
#include "Mesh/Materials/Mechanic/Associative/Steel.h"

#include "Boundary/Boundary.h"
#include "Boundary/Cases/Load_Case.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Types.h"
#include "Analysis/Strategies/Types.h"
#include "Analysis/Solvers/Modal.h"

//ben
#include "benchmarks/mechanic/tensegrity.h"

//model
void tests::tensegrity::modal::pentagon(void)
{
	//data
	const double s0 = 0.05 * 85e6;	//residual stress 5% of 85 MPa
	//const double s0 = 12.5e6;	//residual stress 5% of 250 MPa
	
	//model
	fea::models::Model model("pentagon", "benchmarks/tensegrity/modal");
	fea::mesh::Mesh *mesh = model.mesh();
	
	int nModules = 1;
	double scale = 1.2 / 6.778496; // Coordinates here have a segment length of 6.78m. The real model has a segment length of 1.2m.
	//nodes
	for(int i = 0; i < nModules; i++) {
		double x = (i * 5) * scale;
		if(i == 0) {
			mesh->add_node(+0.000 * scale + x, +0.000 * scale, +3.894 * scale);//nó 0
			mesh->add_node(+0.000 * scale + x, +3.703 * scale, +1.203 * scale);//nó 1
			mesh->add_node(+0.000 * scale + x, +2.289 * scale, -3.150 * scale);//nó 2
			mesh->add_node(+0.000 * scale + x, -2.289 * scale, -3.150 * scale);//nó 3
			mesh->add_node(+0.000 * scale + x, -3.703 * scale, +1.203 * scale);//nó 4
		}
		mesh->add_node(+2.500 * scale + x, +0.000 * scale, -3.894 * scale);//nó 5
		mesh->add_node(+2.500 * scale + x, -3.703 * scale, -1.203 * scale);//nó 6
		mesh->add_node(+2.500 * scale + x, -2.289 * scale, +3.150 * scale);//nó 7
		mesh->add_node(+2.500 * scale + x, +2.289 * scale, +3.150 * scale);//nó 8
		mesh->add_node(+2.500 * scale + x, +3.703 * scale, -1.203 * scale);//nó 9
		mesh->add_node(+5.000 * scale + x, +0.000 * scale, +3.894 * scale);//nó 10
		mesh->add_node(+5.000 * scale + x, +3.703 * scale, +1.203 * scale);//nó 11
		mesh->add_node(+5.000 * scale + x, +2.289 * scale, -3.150 * scale);//nó 12
		mesh->add_node(+5.000 * scale + x, -2.289 * scale, -3.150 * scale);//nó 13
		mesh->add_node(+5.000 * scale + x, -3.703 * scale, +1.203 * scale);//nó 14
	}

	//materials
	fea::mesh::materials::Steel* cableMaterial = (fea::mesh::materials::Steel*)mesh->add_material(fea::mesh::materials::type::steel);
	//cableMaterial->elastic_modulus(115e9); // steel

	// sisal
	cableMaterial->elastic_modulus(1.421e9);
	cableMaterial->break_stress(85e06);
	cableMaterial->break_strain(0.16);
	cableMaterial->yield_stress(85e06);
	cableMaterial->poisson_ratio(0.25);
	cableMaterial->specific_mass(983);

	fea::mesh::materials::Steel* strutMaterial = (fea::mesh::materials::Steel*)mesh->add_material(fea::mesh::materials::type::steel);
	//strutMaterial->elastic_modulus(210e9); // steel

	// bamboo
	strutMaterial->elastic_modulus(19.41e9);
	strutMaterial->break_stress(64.45e06);
	strutMaterial->break_strain(0.075);
	strutMaterial->yield_stress(64.45e06);
	strutMaterial->poisson_ratio(0.3);
	strutMaterial->specific_mass(840.15);

//	//sections
	fea::mesh::sections::Circle *cableSection = (fea::mesh::sections::Circle*)mesh->add_section(fea::mesh::sections::type::circle);
	cableSection->diameter(6./1000);

	fea::mesh::sections::Ring *strutSection = (fea::mesh::sections::Ring*)mesh->add_section(fea::mesh::sections::type::ring); // bamboo strut
	strutSection->diameter(29.3/1000);
	strutSection->thickness(3.8/1000);

//	fea::mesh::sections::Circle *cableSection = (fea::mesh::sections::Circle*)mesh->add_section(fea::mesh::sections::type::circle);
//	cableSection->diameter(2 * sqrt(1.5e-04 / 3.141592)); // steel

//	fea::mesh::sections::Circle *strutSection = (fea::mesh::sections::Circle*)mesh->add_section(fea::mesh::sections::type::circle);
//	strutSection->diameter(2 * sqrt(6e-04 / 3.141592)); // steel

	//cells
	fea::mesh::cells::Line* cableCell = (fea::mesh::cells::Line*)mesh->add_cell(fea::mesh::cells::type::bar);
	cableCell->section(0);
	fea::mesh::cells::Line* strutCell = (fea::mesh::cells::Line*)mesh->add_cell(fea::mesh::cells::type::bar);
	strutCell->section(1);

	//elements
	for(int i = 0; i < nModules; ++i) {
		unsigned int n = i * 10;
		// cables
		if(i == 0) {
			mesh->add_element(fea::mesh::elements::type::bar, { 0,  1}, 0, 0);
			mesh->add_element(fea::mesh::elements::type::bar, { 1,  2}, 0, 0); // 2,3 - 1
			mesh->add_element(fea::mesh::elements::type::bar, { 2,  3}, 0, 0);
			mesh->add_element(fea::mesh::elements::type::bar, { 3,  4}, 0, 0); // 4,5 - 3
			mesh->add_element(fea::mesh::elements::type::bar, { 4,  0}, 0, 0);
		}
		mesh->add_element(fea::mesh::elements::type::bar, {10+n, 11+n}, 0, 0);
		mesh->add_element(fea::mesh::elements::type::bar, {11+n, 12+n}, 0, 0); // 12,13 - 6
		mesh->add_element(fea::mesh::elements::type::bar, {12+n, 13+n}, 0, 0);
		mesh->add_element(fea::mesh::elements::type::bar, {13+n, 14+n}, 0, 0);
		mesh->add_element(fea::mesh::elements::type::bar, {14+n, 10+n}, 0, 0);
		mesh->add_element(fea::mesh::elements::type::bar, { 0+n,  8+n}, 0, 0);
		mesh->add_element(fea::mesh::elements::type::bar, { 1+n,  8+n}, 0, 0); // 2,9 - 11
		mesh->add_element(fea::mesh::elements::type::bar, {10+n,  8+n}, 0, 0);
		mesh->add_element(fea::mesh::elements::type::bar, {11+n,  8+n}, 0, 0);
		mesh->add_element(fea::mesh::elements::type::bar, { 1+n,  9+n}, 0, 0);
		mesh->add_element(fea::mesh::elements::type::bar, { 2+n,  9+n}, 0, 0);
		mesh->add_element(fea::mesh::elements::type::bar, {11+n,  9+n}, 0, 0);
		mesh->add_element(fea::mesh::elements::type::bar, {12+n,  9+n}, 0, 0);
		mesh->add_element(fea::mesh::elements::type::bar, { 2+n,  5+n}, 0, 0);
		mesh->add_element(fea::mesh::elements::type::bar, { 3+n,  5+n}, 0, 0); // 6,4 - 19
		mesh->add_element(fea::mesh::elements::type::bar, {12+n,  5+n}, 0, 0);
		mesh->add_element(fea::mesh::elements::type::bar, {13+n,  5+n}, 0, 0);
		mesh->add_element(fea::mesh::elements::type::bar, { 3+n,  6+n}, 0, 0);
		mesh->add_element(fea::mesh::elements::type::bar, { 4+n,  6+n}, 0, 0);
		mesh->add_element(fea::mesh::elements::type::bar, {13+n,  6+n}, 0, 0);
		mesh->add_element(fea::mesh::elements::type::bar, {14+n,  6+n}, 0, 0);
		mesh->add_element(fea::mesh::elements::type::bar, { 0+n,  7+n}, 0, 0);
		mesh->add_element(fea::mesh::elements::type::bar, { 4+n,  7+n}, 0, 0);
		mesh->add_element(fea::mesh::elements::type::bar, {10+n,  7+n}, 0, 0);
		mesh->add_element(fea::mesh::elements::type::bar, {14+n,  7+n}, 0, 0);

		for(unsigned j = mesh->elements().size() - (i == 0 ? 30 : 25); j < mesh->elements().size(); j++) {
			((fea::mesh::elements::Bar*) mesh->element(j))->cable(true);
			((fea::mesh::elements::Bar*) mesh->element(j))->residual_stress(s0);
		}

//		((fea::mesh::elements::Bar*) mesh->element(1))->residual_stress(s0 / 10.);
//		((fea::mesh::elements::Bar*) mesh->element(3))->residual_stress(s0 / 10.);
//		((fea::mesh::elements::Bar*) mesh->element(6))->residual_stress(s0 / 10.);
//		((fea::mesh::elements::Bar*) mesh->element(11))->residual_stress(s0 / 10.);
//		((fea::mesh::elements::Bar*) mesh->element(19))->residual_stress(s0 / 10.);

		// struts
		mesh->add_element(fea::mesh::elements::type::bar, { 0+n, 11+n}, 1, 1);
		mesh->add_element(fea::mesh::elements::type::bar, { 1+n, 12+n}, 1, 1);
		mesh->add_element(fea::mesh::elements::type::bar, { 2+n, 13+n}, 1, 1);
		mesh->add_element(fea::mesh::elements::type::bar, { 3+n, 14+n}, 1, 1);
		mesh->add_element(fea::mesh::elements::type::bar, { 4+n, 10+n}, 1, 1);
		mesh->add_element(fea::mesh::elements::type::bar, {14+n,  8+n}, 1, 1);
		mesh->add_element(fea::mesh::elements::type::bar, { 8+n,  2+n}, 1, 1);
		mesh->add_element(fea::mesh::elements::type::bar, {10+n,  9+n}, 1, 1);
		mesh->add_element(fea::mesh::elements::type::bar, { 9+n,  3+n}, 1, 1);
		mesh->add_element(fea::mesh::elements::type::bar, {11+n,  5+n}, 1, 1);
		mesh->add_element(fea::mesh::elements::type::bar, { 5+n,  4+n}, 1, 1);
		mesh->add_element(fea::mesh::elements::type::bar, {12+n,  6+n}, 1, 1);
		mesh->add_element(fea::mesh::elements::type::bar, { 6+n,  0+n}, 1, 1);
		mesh->add_element(fea::mesh::elements::type::bar, {13+n,  7+n}, 1, 1);
		mesh->add_element(fea::mesh::elements::type::bar, { 7+n,  1+n}, 1, 1);
	}

	//supports
	int option = 1;
	if(option == 1) {
		model.boundary()->add_support(5, fea::mesh::nodes::dof::translation_x);
		model.boundary()->add_support(5, fea::mesh::nodes::dof::translation_y);
		model.boundary()->add_support(5, fea::mesh::nodes::dof::translation_z);
	}
	else if(option == 2) {
		model.mesh()->add_node(+2.500 * scale, +0.000 * scale, -3.894 * scale - 0.15);//nó 5 para cima 15 cm
		unsigned int lastNodeIndex = model.mesh()->nodes().size() - 1;

		// On the lab, the structure is held only by node 5 (or 5+1=6 there).
		model.boundary()->add_support(lastNodeIndex, fea::mesh::nodes::dof::translation_x);
		model.boundary()->add_support(lastNodeIndex, fea::mesh::nodes::dof::translation_y);
		model.boundary()->add_support(lastNodeIndex, fea::mesh::nodes::dof::translation_z);

		mesh->add_element(fea::mesh::elements::type::bar, {5, lastNodeIndex}, 1, 1);

		//unsigned int lastElementIndex = mesh->elements().size() - 1;
		//((fea::mesh::elements::Bar*) mesh->element(lastElementIndex))->cable(true);
		//((fea::mesh::elements::Bar*) mesh->element(lastElementIndex))->residual_stress(s0);
	}

//	for(int i = 0; i < 5; ++i) {
//		model.boundary()->add_support(i, fea::mesh::nodes::dof::translation_x);
//		model.boundary()->add_support(i, fea::mesh::nodes::dof::translation_y);
//		model.boundary()->add_support(i, fea::mesh::nodes::dof::translation_z);
//	}

//	for(int i = model.mesh()->nodes().size() - 5; i < model.mesh()->nodes().size(); ++i) {
//		model.boundary()->add_support(i, fea::mesh::nodes::dof::translation_x);
//		model.boundary()->add_support(i, fea::mesh::nodes::dof::translation_y);
//		model.boundary()->add_support(i, fea::mesh::nodes::dof::translation_z);
//	}
	
	//solver
	fea::mesh::elements::Mechanic::geometric(true);
	model.analysis()->solver(fea::analysis::solvers::type::modal);
	model.analysis()->solver()->watch_dof(10, fea::mesh::nodes::dof::translation_z);
	dynamic_cast<fea::analysis::solvers::Modal*> (model.analysis()->solver())->scale(scale);
	dynamic_cast<fea::analysis::solvers::Modal*> (model.analysis()->solver())->spectre(fea::analysis::solvers::spectre::full);
	model.analysis()->solve();

	for(int i = 0; i < mesh->nodes().size(); ++i) {
		fea::mesh::nodes::Node *node = mesh->node(i);
	}

	//save
	model.save();
}
