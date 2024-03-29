//std
#include <cmath>

//fea
#include "Model/Model.h"

#include "Mesh/Mesh.h"
#include "Mesh/Nodes/Node.h"
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
#include "Analysis/Solvers/Static_Nonlinear.h"

#include "linear/vec3.h"

//ben
#include "benchmarks/mechanic/tensegrity.h"

double tetrahedronVolume(const mat::vec3& a, const mat::vec3& b, const mat::vec3& c, const mat::vec3& d)
{
	mat::vec3 v1 = a - d;
	mat::vec3 v2 = b - d;
	mat::vec3 v3 = c - d;
	return std::abs(v1.dot(v2.cross(v3))) / 6.;
}

//model
void tests::tensegrity::static_nonlinear::pentagon(double pretension)
{
	//data
	const double s0 = pretension * 85e6;	//residual stress 5% of 85 MPa

	//model
	fea::models::Model model("pentagon", "benchmarks/tensegrity/static/nonlinear");
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
	strutMaterial->elastic_modulus(19.4e9);
	strutMaterial->break_stress(64.5e06);
	strutMaterial->break_strain(0.075);
	strutMaterial->yield_stress(64.5e06);
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
			mesh->add_element(fea::mesh::elements::type::bar, { 1,  2}, 0, 0);
			mesh->add_element(fea::mesh::elements::type::bar, { 2,  3}, 0, 0);
			mesh->add_element(fea::mesh::elements::type::bar, { 3,  4}, 0, 0);
			mesh->add_element(fea::mesh::elements::type::bar, { 4,  0}, 0, 0);
		}
		mesh->add_element(fea::mesh::elements::type::bar, {10+n, 11+n}, 0, 0);
		mesh->add_element(fea::mesh::elements::type::bar, {11+n, 12+n}, 0, 0);
		mesh->add_element(fea::mesh::elements::type::bar, {12+n, 13+n}, 0, 0);
		mesh->add_element(fea::mesh::elements::type::bar, {13+n, 14+n}, 0, 0);
		mesh->add_element(fea::mesh::elements::type::bar, {14+n, 10+n}, 0, 0);
		mesh->add_element(fea::mesh::elements::type::bar, { 0+n,  8+n}, 0, 0);
		mesh->add_element(fea::mesh::elements::type::bar, { 1+n,  8+n}, 0, 0);
		mesh->add_element(fea::mesh::elements::type::bar, {10+n,  8+n}, 0, 0);
		mesh->add_element(fea::mesh::elements::type::bar, {11+n,  8+n}, 0, 0);
		mesh->add_element(fea::mesh::elements::type::bar, { 1+n,  9+n}, 0, 0);
		mesh->add_element(fea::mesh::elements::type::bar, { 2+n,  9+n}, 0, 0);
		mesh->add_element(fea::mesh::elements::type::bar, {11+n,  9+n}, 0, 0);
		mesh->add_element(fea::mesh::elements::type::bar, {12+n,  9+n}, 0, 0);
		mesh->add_element(fea::mesh::elements::type::bar, { 2+n,  5+n}, 0, 0);
		mesh->add_element(fea::mesh::elements::type::bar, { 3+n,  5+n}, 0, 0);
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

		mesh->add_element(fea::mesh::elements::type::bar, {5, lastNodeIndex}, 0, 0);

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
	
	//load cases
	//model.boundary()->add_load_case();
	//	for(unsigned i = 0; i < 15; i++)
	//	{
	//		model.boundary()->load_case(0)->add_load_node((i), fea::mesh::nodes::dof::translation_y, -100);
	//		((fea::mesh::elements::Bar*) model.mesh()->element(i))->residual_stress(115E09 * 0.05);
	//	}

	//carregamento vertical
//	double verServiceLoad = 4 * 1000; // 4 kN/m2
//	double walkingArea = (2.289*2)*5; // m2
//	double nodeVerServiceLoad = walkingArea * verServiceLoad / 4; // spread across 4 nodes
//	double nodeHorServiceLoad = 0.1 * nodeVerServiceLoad;
//	for(int i = 0; i < nModules; ++i) {
//		int n = 10 * i;
//		model.boundary()->load_case(0)->add_load_node(3-1 + n, fea::mesh::nodes::dof::translation_z, -nodeVerServiceLoad);
//		model.boundary()->load_case(0)->add_load_node(4-1 + n, fea::mesh::nodes::dof::translation_z, -nodeVerServiceLoad);
//		model.boundary()->load_case(0)->add_load_node(13-1 + n, fea::mesh::nodes::dof::translation_z, -nodeVerServiceLoad);
//		model.boundary()->load_case(0)->add_load_node(14-1 + n, fea::mesh::nodes::dof::translation_z, -nodeVerServiceLoad);

//		model.boundary()->load_case(0)->add_load_node(3-1 + n, fea::mesh::nodes::dof::translation_x, nodeHorServiceLoad);
//		model.boundary()->load_case(0)->add_load_node(4-1 + n, fea::mesh::nodes::dof::translation_x, nodeHorServiceLoad);
//		model.boundary()->load_case(0)->add_load_node(13-1 + n, fea::mesh::nodes::dof::translation_x, nodeHorServiceLoad);
//		model.boundary()->load_case(0)->add_load_node(14-1 + n, fea::mesh::nodes::dof::translation_x, nodeHorServiceLoad);
//	}

	//model.boundary()->add_self_weight("gravity", fea::mesh::nodes::dof::translation_z);
	model.boundary()->add_self_weight("gravity", fea::mesh::nodes::dof::translation_z, -9.81);
	
	//solver
	fea::mesh::elements::Mechanic::geometric(true);
	model.analysis()->solver(fea::analysis::solvers::type::static_nonlinear);
	model.analysis()->solver()->watch_dof(3, fea::mesh::nodes::dof::translation_z);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->step_max(2000);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_max(1.00);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_predictor(0.05);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->strategy(fea::analysis::strategies::type::control_load);
	model.analysis()->solve();


	mat::vec3 na = scale * mat::vec3(0.0, 0, 0);
	mat::vec3 nb = scale * mat::vec3(2.5, 0, 0);
	mat::vec3 nc = scale * mat::vec3(5.0, 0, 0);
	//std::vector<std::vector<mat::vec3>> nodes;


	std::vector<mat::vec3> vectorNodes;
	for(int i = 0; i < mesh->nodes().size(); ++i) {
		fea::mesh::nodes::Node *node = mesh->node(i);
		double dx = node->state(fea::mesh::nodes::dof::translation_x, 1);
		double dy = node->state(fea::mesh::nodes::dof::translation_y, 1);
		double dz = node->state(fea::mesh::nodes::dof::translation_z, 1);
		double x = node->coordinates()[0] + dx;
		double y = node->coordinates()[1] + dy;
		double z = node->coordinates()[2] + dz;
		vectorNodes.push_back(mat::vec3(x, y, z));

	}

	double volume = 0;
	volume += tetrahedronVolume(vectorNodes[0], vectorNodes[1], na, nb);
	volume += tetrahedronVolume(vectorNodes[1], vectorNodes[2], na, nb);
	volume += tetrahedronVolume(vectorNodes[2], vectorNodes[3], na, nb);
	volume += tetrahedronVolume(vectorNodes[3], vectorNodes[4], na, nb);
	volume += tetrahedronVolume(vectorNodes[4], vectorNodes[0], na, nb);
	volume += tetrahedronVolume(vectorNodes[0], vectorNodes[1], vectorNodes[8], nb);
	volume += tetrahedronVolume(vectorNodes[1], vectorNodes[2], vectorNodes[9], nb);
	volume += tetrahedronVolume(vectorNodes[2], vectorNodes[3], vectorNodes[5], nb);
	volume += tetrahedronVolume(vectorNodes[3], vectorNodes[4], vectorNodes[6], nb);
	volume += tetrahedronVolume(vectorNodes[4], vectorNodes[0], vectorNodes[7], nb);
	volume += tetrahedronVolume(vectorNodes[0], vectorNodes[7], vectorNodes[8], nb);
	volume += tetrahedronVolume(vectorNodes[1], vectorNodes[8], vectorNodes[9], nb);
	volume += tetrahedronVolume(vectorNodes[2], vectorNodes[9], vectorNodes[5], nb);
	volume += tetrahedronVolume(vectorNodes[3], vectorNodes[5], vectorNodes[6], nb);
	volume += tetrahedronVolume(vectorNodes[4], vectorNodes[6], vectorNodes[7], nb);
	volume += tetrahedronVolume(vectorNodes[10], vectorNodes[11], vectorNodes[8], nb);
	volume += tetrahedronVolume(vectorNodes[11], vectorNodes[12], vectorNodes[9], nb);
	volume += tetrahedronVolume(vectorNodes[12], vectorNodes[13], vectorNodes[5], nb);
	volume += tetrahedronVolume(vectorNodes[13], vectorNodes[14], vectorNodes[6], nb);
	volume += tetrahedronVolume(vectorNodes[14], vectorNodes[10], vectorNodes[7], nb);
	volume += tetrahedronVolume(vectorNodes[10], vectorNodes[11], nc, nb);
	volume += tetrahedronVolume(vectorNodes[11], vectorNodes[12], nc, nb);
	volume += tetrahedronVolume(vectorNodes[12], vectorNodes[13], nc, nb);
	volume += tetrahedronVolume(vectorNodes[13], vectorNodes[14], nc, nb);
	volume += tetrahedronVolume(vectorNodes[14], vectorNodes[10], nc, nb);
	volume += tetrahedronVolume(vectorNodes[10], vectorNodes[7], vectorNodes[8], nb);
	volume += tetrahedronVolume(vectorNodes[11], vectorNodes[8], vectorNodes[9], nb);
	volume += tetrahedronVolume(vectorNodes[12], vectorNodes[9], vectorNodes[5], nb);
	volume += tetrahedronVolume(vectorNodes[13], vectorNodes[5], vectorNodes[6], nb);
	volume += tetrahedronVolume(vectorNodes[14], vectorNodes[6], vectorNodes[7], nb);

	//FILE *file = stdout;
	printf("volume: %.6f\n", volume);
	FILE *file = fopen("/home/henrique/pretensions.txt", "a");
	if(file) {
		//fprintf(file, "pretension = %.2f\n", pretension * 100);
		//fprintf(file, "volume = %.6f\n", volume);


		fprintf(file, "%.2f\t%.6f\n", pretension, volume);
		for(int i = 0; i < mesh->nodes().size(); ++i) {
			fea::mesh::nodes::Node *node = mesh->node(i);
			double dx = node->state(fea::mesh::nodes::dof::translation_x, 0);
			double dy = node->state(fea::mesh::nodes::dof::translation_y, 0);
			double dz = node->state(fea::mesh::nodes::dof::translation_z, 0);
			//fprintf(file, "n%d += [%+4.6e, %+4.6e, %+4.6e];\n", i, dx, dy, dz);
		}
		//fprintf(file, "\n");
		fclose(file);
	}

	//save
	model.save();
}
