//std
#include <cmath>

//fea
#include "Model/Model.h"

#include "Mesh/Mesh.h"
#include "Mesh/Nodes/Dofs.h"
#include "Mesh/Cells/Types.h"
#include "Mesh/Joints/Types.h"
#include "Mesh/Joints/Pinned.h"
#include "Mesh/Sections/Types.h"
#include "Mesh/Elements/Types.h"
#include "Mesh/Cells/Line/Line.h"
#include "Mesh/Materials/Types.h"
#include "Mesh/Sections/Generic.h"
#include "Mesh/Elements/Mechanic/Mechanic.h"

#include "Boundary/Boundary.h"
#include "Boundary/Cases/Load_Case.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Types.h"
#include "Analysis/Solvers/Static_Linear.h"

//ben
#include "benchmarks/mechanic/joints.h"

void tests::joint::pinned::frame(void)
{
	/*
	portal frame
	Literature: S. L. Chan, P. P. T. Chui (2000) pp. 49
	 */

	//model
	fea::models::Model model("frame", "benchmarks/joints/pinned");

	//parameters
	const double E = 200e9;
	const double Lc = 6.00e+0;
	const double Ac = 1.10e-2;
	const double Ic = 9.46e-5;
	const double Lb = 1.60e+1;
	const double Ab = 7.60e-3;
	const double Ib = 2.15e-4;
	const double kc = 1.00e+0 * E * Ic / Lc;
	const double kb = 4.00e+0 * E * Ib / Lb;

	//nodes
	model.mesh()->add_node(0,		0,	0);
	model.mesh()->add_node(0,		0,	0);
	model.mesh()->add_node(0,		Lc, 0);
	model.mesh()->add_node(0,		Lc, 0);
	model.mesh()->add_node(Lb / 2,	Lc, 0);
	model.mesh()->add_node(Lb,		Lc, 0);
	model.mesh()->add_node(Lb,		Lc, 0);
	model.mesh()->add_node(Lb,		0, 	0);
	model.mesh()->add_node(Lb,		0, 	0);

	//cells
	model.mesh()->add_cell(fea::mesh::cells::type::beam);
	model.mesh()->add_cell(fea::mesh::cells::type::beam);
	((fea::mesh::cells::Line*) model.mesh()->cell(0))->section(0);
	((fea::mesh::cells::Line*) model.mesh()->cell(1))->section(1);

	//materials
	model.mesh()->add_material(fea::mesh::materials::type::steel);

	//sections
	model.mesh()->add_section(fea::mesh::sections::type::generic);
	model.mesh()->add_section(fea::mesh::sections::type::generic);
	((fea::mesh::sections::Generic*) model.mesh()->section(0))->area(Ac);
	((fea::mesh::sections::Generic*) model.mesh()->section(1))->area(Ab);
	((fea::mesh::sections::Generic*) model.mesh()->section(0))->inertia_z(Ic);
	((fea::mesh::sections::Generic*) model.mesh()->section(1))->inertia_z(Ib);

	//elements
	model.mesh()->add_element(fea::mesh::elements::type::beam2, {1, 2}, { 0 }, 0);
	model.mesh()->add_element(fea::mesh::elements::type::beam2, {3, 4}, { 0 }, 1);
	model.mesh()->add_element(fea::mesh::elements::type::beam2, {4, 5}, { 0 }, 1);
	model.mesh()->add_element(fea::mesh::elements::type::beam2, {6, 7}, { 0 }, 0);

	//joints
	model.mesh()->add_joint(fea::mesh::joints::type::pinned, {0, 1});
	model.mesh()->add_joint(fea::mesh::joints::type::pinned, {2, 3});
	model.mesh()->add_joint(fea::mesh::joints::type::pinned, {5, 6});
	model.mesh()->add_joint(fea::mesh::joints::type::pinned, {7, 8});
	((fea::mesh::joints::Pinned*) model.mesh()->joint(0))->stiffness(kc);
	((fea::mesh::joints::Pinned*) model.mesh()->joint(1))->stiffness(kb);
	((fea::mesh::joints::Pinned*) model.mesh()->joint(2))->stiffness(kb);
	((fea::mesh::joints::Pinned*) model.mesh()->joint(3))->stiffness(kc);

	//supports
	model.boundary()->add_support(0, fea::mesh::nodes::dof::rotation_z);
	model.boundary()->add_support(8, fea::mesh::nodes::dof::rotation_z);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(0, fea::mesh::nodes::dof::translation_y);
	model.boundary()->add_support(8, fea::mesh::nodes::dof::translation_x);
	model.boundary()->add_support(8, fea::mesh::nodes::dof::translation_y);

	//loads
	model.boundary()->add_load_case();
	model.boundary()->load_case(0)->add_load_node(2, fea::mesh::nodes::dof::translation_x, +1.0e4);
	model.boundary()->load_case(0)->add_load_node(4, fea::mesh::nodes::dof::translation_y, -1.0e5);

	//solver
	model.analysis()->solver(fea::analysis::solvers::type::static_linear);
	model.analysis()->solver()->watch_dof(4, fea::mesh::nodes::dof::translation_y);

	//solve
	model.analysis()->solve();
	
	//save
	model.save();
}
