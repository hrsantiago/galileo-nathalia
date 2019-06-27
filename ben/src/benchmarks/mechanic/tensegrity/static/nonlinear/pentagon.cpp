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

//ben
#include "benchmarks/mechanic/tensegrity.h"

//model
void tests::tensegrity::static_nonlinear::pentagon(void)
{
	//data
	const double s0 = 12.5e6;	//residual stress 5% of 250 MPa
	const double E0 = 115e9;	//elastic modulus: cable
	const double E1 = 210e9;	//elastic modulus: strut
	const double A0 = 1.5e-4;	//cross section area: cable
	const double A1 = 6.0e-4;	//cross section area: strut
	
	//model
	fea::models::Model model("pentagon", "benchmarks/tensegrity/static/nonlinear");
	
	int nModules = 4;
	//nodes
	for(int i = 0; i < nModules; i++) {
		double x = i * 5;
		if(i == 0) {
			model.mesh()->add_node(+0.000 + x, +0.000, +3.894);//nó 0
			model.mesh()->add_node(+0.000 + x, +3.703, +1.203);//nó 1
			model.mesh()->add_node(+0.000 + x, +2.289, -3.150);//nó 2
			model.mesh()->add_node(+0.000 + x, -2.289, -3.150);//nó 3
			model.mesh()->add_node(+0.000 + x, -3.703, +1.203);//nó 4
		}
		model.mesh()->add_node(+2.500 + x, +0.000, -3.894);//nó 5
		model.mesh()->add_node(+2.500 + x, -3.703, -1.203);//nó 6
		model.mesh()->add_node(+2.500 + x, -2.289, +3.150);//nó 7
		model.mesh()->add_node(+2.500 + x, +2.289, +3.150);//nó 8
		model.mesh()->add_node(+2.500 + x, +3.703, -1.203);//nó 9
		model.mesh()->add_node(+5.000 + x, +0.000, +3.894);//nó 10
		model.mesh()->add_node(+5.000 + x, +3.703, +1.203);//nó 11
		model.mesh()->add_node(+5.000 + x, +2.289, -3.150);//nó 12
		model.mesh()->add_node(+5.000 + x, -2.289, -3.150);//nó 13
		model.mesh()->add_node(+5.000 + x, -3.703, +1.203);//nó 14
	}

	//cells
	model.mesh()->add_cell(fea::mesh::cells::type::bar);
	model.mesh()->add_cell(fea::mesh::cells::type::bar);
	((fea::mesh::cells::Line*) model.mesh()->cell(0))->section(0);
	((fea::mesh::cells::Line*) model.mesh()->cell(1))->section(1);

	//materials
	model.mesh()->add_material(fea::mesh::materials::type::steel);
	model.mesh()->add_material(fea::mesh::materials::type::steel);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->elastic_modulus(E0);
	((fea::mesh::materials::Steel*) model.mesh()->material(1))->elastic_modulus(E1);

	//sections
	model.mesh()->add_section(fea::mesh::sections::type::circle);
	model.mesh()->add_section(fea::mesh::sections::type::circle);
	((fea::mesh::sections::Circle*) model.mesh()->section(0))->diameter(2 * sqrt(A0 / M_PI));
	((fea::mesh::sections::Circle*) model.mesh()->section(1))->diameter(2 * sqrt(A1 / M_PI));

	//elements
	for(int i = 0; i < nModules; ++i) {
		unsigned int n = i * 10;
		// cables
		if(i == 0) {
			model.mesh()->add_element(fea::mesh::elements::type::bar, { 0,  1}, 0, 0);
			model.mesh()->add_element(fea::mesh::elements::type::bar, { 1,  2}, 0, 0);
			model.mesh()->add_element(fea::mesh::elements::type::bar, { 2,  3}, 0, 0);
			model.mesh()->add_element(fea::mesh::elements::type::bar, { 3,  4}, 0, 0);
			model.mesh()->add_element(fea::mesh::elements::type::bar, { 4,  0}, 0, 0);
		}
		model.mesh()->add_element(fea::mesh::elements::type::bar, {10+n, 11+n}, 0, 0);
		model.mesh()->add_element(fea::mesh::elements::type::bar, {11+n, 12+n}, 0, 0);
		model.mesh()->add_element(fea::mesh::elements::type::bar, {12+n, 13+n}, 0, 0);
		model.mesh()->add_element(fea::mesh::elements::type::bar, {13+n, 14+n}, 0, 0);
		model.mesh()->add_element(fea::mesh::elements::type::bar, {14+n, 10+n}, 0, 0);
		model.mesh()->add_element(fea::mesh::elements::type::bar, { 0+n,  8+n}, 0, 0);
		model.mesh()->add_element(fea::mesh::elements::type::bar, { 1+n,  8+n}, 0, 0);
		model.mesh()->add_element(fea::mesh::elements::type::bar, {10+n,  8+n}, 0, 0);
		model.mesh()->add_element(fea::mesh::elements::type::bar, {11+n,  8+n}, 0, 0);
		model.mesh()->add_element(fea::mesh::elements::type::bar, { 1+n,  9+n}, 0, 0);
		model.mesh()->add_element(fea::mesh::elements::type::bar, { 2+n,  9+n}, 0, 0);
		model.mesh()->add_element(fea::mesh::elements::type::bar, {11+n,  9+n}, 0, 0);
		model.mesh()->add_element(fea::mesh::elements::type::bar, {12+n,  9+n}, 0, 0);
		model.mesh()->add_element(fea::mesh::elements::type::bar, { 2+n,  5+n}, 0, 0);
		model.mesh()->add_element(fea::mesh::elements::type::bar, { 3+n,  5+n}, 0, 0);
		model.mesh()->add_element(fea::mesh::elements::type::bar, {12+n,  5+n}, 0, 0);
		model.mesh()->add_element(fea::mesh::elements::type::bar, {13+n,  5+n}, 0, 0);
		model.mesh()->add_element(fea::mesh::elements::type::bar, { 3+n,  6+n}, 0, 0);
		model.mesh()->add_element(fea::mesh::elements::type::bar, { 4+n,  6+n}, 0, 0);
		model.mesh()->add_element(fea::mesh::elements::type::bar, {13+n,  6+n}, 0, 0);
		model.mesh()->add_element(fea::mesh::elements::type::bar, {14+n,  6+n}, 0, 0);
		model.mesh()->add_element(fea::mesh::elements::type::bar, { 0+n,  7+n}, 0, 0);
		model.mesh()->add_element(fea::mesh::elements::type::bar, { 4+n,  7+n}, 0, 0);
		model.mesh()->add_element(fea::mesh::elements::type::bar, {10+n,  7+n}, 0, 0);
		model.mesh()->add_element(fea::mesh::elements::type::bar, {14+n,  7+n}, 0, 0);

		for(unsigned j = model.mesh()->elements() - (i == 0 ? 30 : 25); j < model.mesh()->elements(); j++) {
			((fea::mesh::elements::Bar*) model.mesh()->element(j))->cable(true);
			((fea::mesh::elements::Bar*) model.mesh()->element(j))->residual_stress(s0);
		}

		// struts
		model.mesh()->add_element(fea::mesh::elements::type::bar, { 0+n, 11+n}, 1, 1);
		model.mesh()->add_element(fea::mesh::elements::type::bar, { 1+n, 12+n}, 1, 1);
		model.mesh()->add_element(fea::mesh::elements::type::bar, { 2+n, 13+n}, 1, 1);
		model.mesh()->add_element(fea::mesh::elements::type::bar, { 3+n, 14+n}, 1, 1);
		model.mesh()->add_element(fea::mesh::elements::type::bar, { 4+n, 10+n}, 1, 1);
		model.mesh()->add_element(fea::mesh::elements::type::bar, {14+n,  8+n}, 1, 1);
		model.mesh()->add_element(fea::mesh::elements::type::bar, { 8+n,  2+n}, 1, 1);
		model.mesh()->add_element(fea::mesh::elements::type::bar, {10+n,  9+n}, 1, 1);
		model.mesh()->add_element(fea::mesh::elements::type::bar, { 9+n,  3+n}, 1, 1);
		model.mesh()->add_element(fea::mesh::elements::type::bar, {11+n,  5+n}, 1, 1);
		model.mesh()->add_element(fea::mesh::elements::type::bar, { 5+n,  4+n}, 1, 1);
		model.mesh()->add_element(fea::mesh::elements::type::bar, {12+n,  6+n}, 1, 1);
		model.mesh()->add_element(fea::mesh::elements::type::bar, { 6+n,  0+n}, 1, 1);
		model.mesh()->add_element(fea::mesh::elements::type::bar, {13+n,  7+n}, 1, 1);
		model.mesh()->add_element(fea::mesh::elements::type::bar, { 7+n,  1+n}, 1, 1);
	}

	//supports
	for(int i = 0; i < 5; ++i) {
		model.boundary()->add_support(i, fea::mesh::nodes::dof::translation_x);
		model.boundary()->add_support(i, fea::mesh::nodes::dof::translation_y);
		model.boundary()->add_support(i, fea::mesh::nodes::dof::translation_z);
	}

	for(int i = model.mesh()->nodes() - 5; i < model.mesh()->nodes(); ++i) {
		model.boundary()->add_support(i, fea::mesh::nodes::dof::translation_x);
		model.boundary()->add_support(i, fea::mesh::nodes::dof::translation_y);
		model.boundary()->add_support(i, fea::mesh::nodes::dof::translation_z);
	}
	
	//load cases
	model.boundary()->add_load_case();
	//	for(unsigned i = 0; i < 15; i++)
	//	{
	//		model.boundary()->load_case(0)->add_load_node((i), fea::mesh::nodes::dof::translation_y, -100);
	//		((fea::mesh::elements::Bar*) model.mesh()->element(i))->residual_stress(115E09 * 0.05);
	//	}

	//carregamento vertical
	double verServiceLoad = 4 * 1000; // 4 kN/m2
	double walkingArea = (2.289*2)*5; // m2
	double nodeVerServiceLoad = walkingArea * verServiceLoad / 4; // spread across 4 nodes
	double nodeHorServiceLoad = 0.1 * nodeVerServiceLoad;
	for(int i = 0; i < nModules; ++i) {
		int n = 10 * i;
		model.boundary()->load_case(0)->add_load_node(3-1 + n, fea::mesh::nodes::dof::translation_z, -nodeVerServiceLoad);
		model.boundary()->load_case(0)->add_load_node(4-1 + n, fea::mesh::nodes::dof::translation_z, -nodeVerServiceLoad);
		model.boundary()->load_case(0)->add_load_node(13-1 + n, fea::mesh::nodes::dof::translation_z, -nodeVerServiceLoad);
		model.boundary()->load_case(0)->add_load_node(14-1 + n, fea::mesh::nodes::dof::translation_z, -nodeVerServiceLoad);

		model.boundary()->load_case(0)->add_load_node(3-1 + n, fea::mesh::nodes::dof::translation_x, nodeHorServiceLoad);
		model.boundary()->load_case(0)->add_load_node(4-1 + n, fea::mesh::nodes::dof::translation_x, nodeHorServiceLoad);
		model.boundary()->load_case(0)->add_load_node(13-1 + n, fea::mesh::nodes::dof::translation_x, nodeHorServiceLoad);
		model.boundary()->load_case(0)->add_load_node(14-1 + n, fea::mesh::nodes::dof::translation_x, nodeHorServiceLoad);
	}

	model.boundary()->add_self_weight("gravity", fea::mesh::nodes::dof::translation_z);
	
	//solver
	fea::mesh::elements::Mechanic::geometric(true);
	model.analysis()->solver(fea::analysis::solvers::type::static_nonlinear);
	model.analysis()->solver()->watch_dof(22, fea::mesh::nodes::dof::translation_z);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->step_max(2000);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_max(1.00);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->load_predictor(0.05);
	dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model.analysis()->solver())->strategy(fea::analysis::strategies::type::control_load);
	model.analysis()->solve();

	//save
	model.save();
}
