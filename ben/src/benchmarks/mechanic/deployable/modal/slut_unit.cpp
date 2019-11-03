//std
#include <cmath>

//mat
#include "misc/util.h"
#include "linear/dense.h"

//fea
#include "Model/Model.h"

#include "Mesh/Mesh.h"
#include "Mesh/Nodes/Dofs.h"
#include "Mesh/Cells/Types.h"
#include "Mesh/Joints/Types.h"
#include "Mesh/Joints/Hinge.h"
#include "Mesh/Sections/Box.h"
#include "Mesh/Sections/Types.h"
#include "Mesh/Elements/Types.h"
#include "Mesh/Cells/Line/Line.h"
#include "Mesh/Materials/Types.h"
#include "Mesh/Elements/Mechanic/Mechanic.h"
#include "Mesh/Elements/Mechanic/Frame/Beam3.h"
#include "Mesh/Materials/Mechanic/Associative/Steel.h"

#include "Boundary/Boundary.h"
#include "Boundary/Cases/Load_Case.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Types.h"
#include "Analysis/Solvers/Modal.h"

//ben
#include "benchmarks/mechanic/deployable.h"

//controls
const static bool open = true;
const static bool fric = false;

//geometry
const static double L = 1.00e+00;
const static double h = 0.50e+00;
const static double H = 0.40e+00;

const static double r = 5.00e-02;
const static double s = 1.00e-02;

//joint
const static double mp = 5.00e+00;
const static double kt = 1.00e+03;
const static double kp = 0.00e+00;
const static double kr = 1.00e+05;

//material
const static double p = 2.70e+03;
const static double E = 7.00e+10;

//cross section
const static double hs = 4.00e-02;
const static double ws = 1.00e-02;
const static double ts = 1.00e-03;

//analysis
const static double Pr = 1.00e+00;
const static double dP = 1.00e+01;
const static double dp = 5.00e-01;
const static double pl = 1.00e+04;
const static double tl = 1.00e-04;

const static unsigned nd = 4;
const static unsigned nr = 4;
const static unsigned ni = 6;
const static unsigned nm = 10;
const static unsigned ns = 250;

//parameters
const static double D = 2 * L * sin(M_PI / nd);
const static double v = r * (4 * sin(M_PI / nd) - 2);
const static double l = sqrt(h * h + pow(L - 2 * r, 2));
const static double e = sqrt(H * H + pow(D - 2 * r, 2)) / 2;
const static double x = (pow(D - 2 * r, 2) - v * v) / (2 * sqrt(4 * e * e - v * v) * l - 2 * h * H);

const static double d = x * l;
const static double b = (1 - x) * l;
const static double z = h - (1 - x) / x * H;
const static double c = sqrt(4 * e * e - v * v) - d;
const static double a = (1 - x) / x * c;

//data
static double p0[3], p1[3];
static double pa[nd][2][3], pb[nd][2][3], po[nd][2][3];
static double pc[nd][2][3], pd[nd][7][3], pe[nd][7][3];

//create
static void set_points(void)
{
	double t;
	p0[0] = p0[1] = 0;
	p1[0] = p1[1] = 0;
	p0[2] = open ? z : d - a;
	p1[2] = open ? h : d + b;
	for(unsigned i = 0; i < nd; i++)
	{
		t = (2 * i + 1) * M_PI / nd;
		pa[i][1][0] = r * cos(t) + s * sin(t);
		pa[i][1][1] = r * sin(t) - s * cos(t);
		pb[i][1][0] = r * cos(t) - s * sin(t);
		pb[i][1][1] = r * sin(t) + s * cos(t);
		pa[i][0][0] = pb[i][0][0] = r * cos(t);
		pa[i][0][1] = pb[i][0][1] = r * sin(t);
		pa[i][0][2] = pa[i][1][2] = open ? z : d - a;
		pb[i][0][2] = pb[i][1][2] = open ? h : d + b;
		po[i][0][2] = po[i][1][2] = open ? x * h : d;
		pc[i][0][2] = pc[i][1][2] = open ? H / 2 : (d + c) / 2;
		pd[i][4][0] = (open ? L - r : r) * cos(t) + s * sin(t);
		pd[i][4][1] = (open ? L - r : r) * sin(t) - s * cos(t);
		pe[i][4][0] = (open ? L - r : r) * cos(t) - s * sin(t);
		pe[i][4][1] = (open ? L - r : r) * sin(t) + s * cos(t);
		pd[i][0][0] = pe[i][0][0] = (open ? L : 2 * r) * cos(t);
		pd[i][0][1] = pe[i][0][1] = (open ? L : 2 * r) * sin(t);
		pd[i][1][0] = pe[i][1][0] = (open ? L - r : r) * cos(t);
		pd[i][1][1] = pe[i][1][1] = (open ? L - r : r) * sin(t);
		po[i][0][0] = (open ? r + (1 - x) * (L - 2 * r) : r) * cos(t) + s * sin(t);
		po[i][0][1] = (open ? r + (1 - x) * (L - 2 * r) : r) * sin(t) - s * cos(t);
		po[i][1][0] = (open ? r + (1 - x) * (L - 2 * r) : r) * cos(t) - s * sin(t);
		po[i][1][1] = (open ? r + (1 - x) * (L - 2 * r) : r) * sin(t) + s * cos(t);
		pc[i][0][0] = ((open ? L : 2 * r) * cos(M_PI / nd) - s) * cos(t + M_PI / nd);
		pc[i][0][1] = ((open ? L : 2 * r) * cos(M_PI / nd) - s) * sin(t + M_PI / nd);
		pc[i][1][0] = ((open ? L : 2 * r) * cos(M_PI / nd) + s) * cos(t + M_PI / nd);
		pc[i][1][1] = ((open ? L : 2 * r) * cos(M_PI / nd) + s) * sin(t + M_PI / nd);
		pd[i][2][0] = pe[i][2][0] = (open ? L : 2 * r) * cos(t) + r * sin(t - M_PI / nd);
		pd[i][2][1] = pe[i][2][1] = (open ? L : 2 * r) * sin(t) - r * cos(t - M_PI / nd);
		pd[i][3][0] = pe[i][3][0] = (open ? L : 2 * r) * cos(t) - r * sin(t + M_PI / nd);
		pd[i][3][1] = pe[i][3][1] = (open ? L : 2 * r) * sin(t) + r * cos(t + M_PI / nd);
		pd[i][5][0] = (open ? L : 2 * r) * cos(t) + r * sin(t - M_PI / nd) + s * cos(t - M_PI / nd);
		pd[i][5][1] = (open ? L : 2 * r) * sin(t) - r * cos(t - M_PI / nd) + s * sin(t - M_PI / nd);
		pd[i][6][0] = (open ? L : 2 * r) * cos(t) - r * sin(t + M_PI / nd) - s * cos(t + M_PI / nd);
		pd[i][6][1] = (open ? L : 2 * r) * sin(t) + r * cos(t + M_PI / nd) - s * sin(t + M_PI / nd);
		pe[i][5][0] = (open ? L : 2 * r) * cos(t) + r * sin(t - M_PI / nd) - s * cos(t - M_PI / nd);
		pe[i][5][1] = (open ? L : 2 * r) * sin(t) - r * cos(t - M_PI / nd) - s * sin(t - M_PI / nd);
		pe[i][6][0] = (open ? L : 2 * r) * cos(t) - r * sin(t + M_PI / nd) + s * cos(t + M_PI / nd);
		pe[i][6][1] = (open ? L : 2 * r) * sin(t) + r * cos(t + M_PI / nd) + s * sin(t + M_PI / nd);
		pe[i][0][2] = pe[i][1][2] = pe[i][2][2] = pe[i][3][2] = pe[i][4][2] = pe[i][5][2] = pe[i][6][2] = 0;
		pd[i][0][2] = pd[i][1][2] = pd[i][2][2] = pd[i][3][2] = pd[i][4][2] = pd[i][5][2] = pd[i][6][2] = open ? H : d + c;
	}
}
static void check_points(void)
{
	//mass
	const double A = 2 * (ws + hs - 2 * ts) * ts;
	printf("Mass:\t%+.2e\n", p * A * (4 * (4 * r + a + b + c + d) + 16 * (e + r)));
	
	//check
	for(unsigned i = 0; i < nd; i++)
	{
		printf("check %d: ", i);
		const unsigned j = i + 1 != nd ? i + 1 : 0;
		printf("%d ", fabs(mat::norm(pa[i][0], p0, 3) - r) < 1e-5 * L);
		printf("%d ", fabs(mat::norm(pb[i][0], p1, 3) - r) < 1e-5 * L);
		printf("%d ", fabs(mat::norm(pa[i][1], po[i][0], 3) - a) < 1e-5 * L);
		printf("%d ", fabs(mat::norm(pb[i][1], po[i][1], 3) - b) < 1e-5 * L);
		printf("%d ", fabs(mat::norm(pd[i][4], po[i][0], 3) - c) < 1e-5 * L);
		printf("%d ", fabs(mat::norm(pe[i][4], po[i][1], 3) - d) < 1e-5 * L);
		printf("%d ", fabs(mat::norm(pd[i][0], pd[i][1], 3) - r) < 1e-5 * L);
		printf("%d ", fabs(mat::norm(pd[i][0], pd[i][2], 3) - r) < 1e-5 * L);
		printf("%d ", fabs(mat::norm(pd[i][0], pd[i][3], 3) - r) < 1e-5 * L);
		printf("%d ", fabs(mat::norm(pe[i][0], pe[i][1], 3) - r) < 1e-5 * L);
		printf("%d ", fabs(mat::norm(pe[i][0], pe[i][2], 3) - r) < 1e-5 * L);
		printf("%d ", fabs(mat::norm(pe[i][0], pe[i][3], 3) - r) < 1e-5 * L);
		printf("%d ", fabs(mat::norm(pa[i][0], pa[i][1], 3) - s) < 1e-5 * L);
		printf("%d ", fabs(mat::norm(pb[i][0], pb[i][1], 3) - s) < 1e-5 * L);
		printf("%d ", fabs(mat::norm(pd[i][1], pd[i][4], 3) - s) < 1e-5 * L);
		printf("%d ", fabs(mat::norm(pd[i][2], pd[i][5], 3) - s) < 1e-5 * L);
		printf("%d ", fabs(mat::norm(pd[i][3], pd[i][6], 3) - s) < 1e-5 * L);
		printf("%d ", fabs(mat::norm(pe[i][1], pe[i][4], 3) - s) < 1e-5 * L);
		printf("%d ", fabs(mat::norm(pe[i][2], pe[i][5], 3) - s) < 1e-5 * L);
		printf("%d ", fabs(mat::norm(pe[i][3], pe[i][6], 3) - s) < 1e-5 * L);
		printf("%d ", fabs(mat::norm(pd[i][6], pc[i][0], 3) - e) < 1e-5 * L);
		printf("%d ", fabs(mat::norm(pe[i][6], pc[i][1], 3) - e) < 1e-5 * L);
		printf("%d ", fabs(mat::norm(pd[j][5], pc[i][1], 3) - e) < 1e-5 * L);
		printf("%d ", fabs(mat::norm(pe[j][5], pc[i][0], 3) - e) < 1e-5 * L);
		printf("%d ", fabs(mat::norm(po[i][0], po[i][1], 3) - 2 * s) < 1e-5 * L);
		printf("%d ", fabs(mat::norm(pc[i][0], pc[i][1], 3) - 2 * s) < 1e-5 * L);
		printf("\n");
	}
}

static void create_nodes(fea::models::Model* model)
{
	model->mesh()->add_node(p0);
	model->mesh()->add_node(p1);
	for(unsigned i = 0; i < nd; i++)
	{
		model->mesh()->add_node(pa[i][0]);
		model->mesh()->add_node(pa[i][1]);
		model->mesh()->add_node(pb[i][0]);
		model->mesh()->add_node(pb[i][1]);
		model->mesh()->add_node(po[i][0]);
		model->mesh()->add_node(po[i][1]);
		model->mesh()->add_node(pc[i][0]);
		model->mesh()->add_node(pc[i][1]);
		model->mesh()->add_node(pd[i][0]);
		model->mesh()->add_node(pd[i][1]);
		model->mesh()->add_node(pd[i][2]);
		model->mesh()->add_node(pd[i][3]);
		model->mesh()->add_node(pd[i][4]);
		model->mesh()->add_node(pd[i][5]);
		model->mesh()->add_node(pd[i][6]);
		model->mesh()->add_node(pe[i][0]);
		model->mesh()->add_node(pe[i][1]);
		model->mesh()->add_node(pe[i][2]);
		model->mesh()->add_node(pe[i][3]);
		model->mesh()->add_node(pe[i][4]);
		model->mesh()->add_node(pe[i][5]);
		model->mesh()->add_node(pe[i][6]);
	}
}
static void create_cells(fea::models::Model* model)
{
	model->mesh()->add_cell(fea::mesh::cells::type::beam);
}
static void create_joints(fea::models::Model* model)
{
	for(unsigned i = 0; i < nd; i++)
	{
		const double t = (2 * i + 1) * M_PI / nd;
		const double s1[] = {+cos(t), +sin(t), 0};
		const double s2[] = {-sin(t), +cos(t), 0};
		const double u1[] = {+cos(t - M_PI / nd), +sin(t - M_PI / nd), 0};
		const double u2[] = {-sin(t - M_PI / nd), +cos(t - M_PI / nd), 0};
		const double e1[] = {+cos(t + M_PI / nd), +sin(t + M_PI / nd), 0};
		const double e2[] = {-sin(t + M_PI / nd), +cos(t + M_PI / nd), 0};
		const double* ha[] = {s2, s2, s2, e1, s2, u1, e1, s2, u1, e1};
		const double* ho[] = {s1, s1, s1, e2, s1, u2, e2, s1, u2, e2}; 
		model->mesh()->add_joint(fea::mesh::joints::type::hinge, {22 * i +  2, 22 * i +  3});
		model->mesh()->add_joint(fea::mesh::joints::type::hinge, {22 * i +  4, 22 * i +  5});
		model->mesh()->add_joint(fea::mesh::joints::type::hinge, {22 * i +  6, 22 * i +  7});
		model->mesh()->add_joint(fea::mesh::joints::type::hinge, {22 * i +  8, 22 * i +  9});
		model->mesh()->add_joint(fea::mesh::joints::type::hinge, {22 * i + 11, 22 * i + 14});
		model->mesh()->add_joint(fea::mesh::joints::type::hinge, {22 * i + 12, 22 * i + 15});
		model->mesh()->add_joint(fea::mesh::joints::type::hinge, {22 * i + 13, 22 * i + 16});
		model->mesh()->add_joint(fea::mesh::joints::type::hinge, {22 * i + 18, 22 * i + 21});
		model->mesh()->add_joint(fea::mesh::joints::type::hinge, {22 * i + 19, 22 * i + 22});
		model->mesh()->add_joint(fea::mesh::joints::type::hinge, {22 * i + 20, 22 * i + 23});
		for(unsigned j = 0; j < 10; j++)
		{
			((fea::mesh::joints::Hinge*) model->mesh()->joint(10 * i + j))->axis(ha[j]);
			((fea::mesh::joints::Hinge*) model->mesh()->joint(10 * i + j))->orientation(ho[j]);
			((fea::mesh::joints::Hinge*) model->mesh()->joint(10 * i + j))->stiffness(0, 0, kr);
			((fea::mesh::joints::Hinge*) model->mesh()->joint(10 * i + j))->stiffness(0, 1, kr);
			((fea::mesh::joints::Hinge*) model->mesh()->joint(10 * i + j))->moment_yield(fric ? mp : 1);
			((fea::mesh::joints::Hinge*) model->mesh()->joint(10 * i + j))->plastic_modulus(fric ? kp : 0);
			((fea::mesh::joints::Hinge*) model->mesh()->joint(10 * i + j))->stiffness(0, 2, fric ? kt : 0);
		}
	}
}
static void create_sections(fea::models::Model* model)
{
	model->mesh()->add_section(fea::mesh::sections::type::box);
	((fea::mesh::sections::Box*) model->mesh()->section(0))->width(ws);
	((fea::mesh::sections::Box*) model->mesh()->section(0))->height(hs);
	((fea::mesh::sections::Box*) model->mesh()->section(0))->thickness(ts);
}
static void create_elements(fea::models::Model* model)
{
	for(unsigned i = 0; i < nd; i++)
	{
		const double t = (2 * i + 1) * M_PI / nd;
		const double s2[] = {-sin(t), +cos(t), 0};
		const double u1[] = {+cos(t - M_PI / nd), +sin(t - M_PI / nd), 0};
		const double e1[] = {+cos(t + M_PI / nd), +sin(t + M_PI / nd), 0};
		const double* bo[] = {s2, s2, s2, s2, s2, s2, e1, e1, s2, u1, e1, s2, u1, e1, e1, e1};
		model->mesh()->add_element(fea::mesh::elements::type::beam3, {22 * i +  2, 0});
		model->mesh()->add_element(fea::mesh::elements::type::beam3, {22 * i +  4, 1});
		model->mesh()->add_element(fea::mesh::elements::type::beam3, {22 * i +  3, 22 * i +  6});
		model->mesh()->add_element(fea::mesh::elements::type::beam3, {22 * i +  5, 22 * i +  7});
		model->mesh()->add_element(fea::mesh::elements::type::beam3, {22 * i +  6, 22 * i + 14});
		model->mesh()->add_element(fea::mesh::elements::type::beam3, {22 * i +  7, 22 * i + 21});
		model->mesh()->add_element(fea::mesh::elements::type::beam3, {22 * i +  8, 22 * i + 16});
		model->mesh()->add_element(fea::mesh::elements::type::beam3, {22 * i +  9, 22 * i + 23});
		model->mesh()->add_element(fea::mesh::elements::type::beam3, {22 * i + 10, 22 * i + 11});
		model->mesh()->add_element(fea::mesh::elements::type::beam3, {22 * i + 10, 22 * i + 12});
		model->mesh()->add_element(fea::mesh::elements::type::beam3, {22 * i + 10, 22 * i + 13});
		model->mesh()->add_element(fea::mesh::elements::type::beam3, {22 * i + 17, 22 * i + 18});
		model->mesh()->add_element(fea::mesh::elements::type::beam3, {22 * i + 17, 22 * i + 19});
		model->mesh()->add_element(fea::mesh::elements::type::beam3, {22 * i + 17, 22 * i + 20});
		model->mesh()->add_element(fea::mesh::elements::type::beam3, {22 * i +  8, 22 * (i + 1 != nd ? i + 1 : 0) + 22});
		model->mesh()->add_element(fea::mesh::elements::type::beam3, {22 * i +  9, 22 * (i + 1 != nd ? i + 1 : 0) + 15});
		for(unsigned j = 0; j < 16; j++)
		{
			((fea::mesh::elements::Beam3*) model->mesh()->element(16 * i + j))->orientation(bo[j]);
		}
	}
}
static void refine_elements(fea::models::Model* model)
{
	for(unsigned i = 0; i < nd; i++)
	{
		fea::mesh::cells::Line::refine(16 * i +  2, nr);
		fea::mesh::cells::Line::refine(16 * i +  3, nr);
		fea::mesh::cells::Line::refine(16 * i +  4, nr);
		fea::mesh::cells::Line::refine(16 * i +  5, nr);
		fea::mesh::cells::Line::refine(16 * i +  6, nr);
		fea::mesh::cells::Line::refine(16 * i +  7, nr);
		fea::mesh::cells::Line::refine(16 * i + 14, nr);
		fea::mesh::cells::Line::refine(16 * i + 15, nr);
	}
}
static void create_materials(fea::models::Model* model)
{
	model->mesh()->add_material(fea::mesh::materials::type::steel);
	((fea::mesh::materials::Steel*) model->mesh()->material(0))->specific_mass(p);
	((fea::mesh::materials::Steel*) model->mesh()->material(0))->elastic_modulus(E);
}

static void create_supports(fea::models::Model* model)
{
	model->boundary()->add_support(0, fea::mesh::nodes::dof::rotation_x);
	model->boundary()->add_support(0, fea::mesh::nodes::dof::rotation_y);
	model->boundary()->add_support(0, fea::mesh::nodes::dof::rotation_z);
	model->boundary()->add_support(0, fea::mesh::nodes::dof::translation_x);
	model->boundary()->add_support(0, fea::mesh::nodes::dof::translation_y);
	model->boundary()->add_support(0, fea::mesh::nodes::dof::translation_z);
}

static void create_solver(fea::models::Model* model)
{
	model->analysis()->solver(fea::analysis::solvers::type::modal);
	model->analysis()->solver()->watch_dof(1, fea::mesh::nodes::dof::translation_z);
	model->analysis()->solve();
}

//model
void tests::deployable::modal::slut_unit(void)
{
	//model
	fea::models::Model model("slut unit", "benchmarks/deployable/modal");
	
	//mesh
	set_points();
	check_points();
	create_nodes(&model);
	create_cells(&model);
	create_joints(&model);
	create_sections(&model);
	create_elements(&model);
	refine_elements(&model);
	create_materials(&model);
	
	//boundary
	create_supports(&model);

	//analysis
	create_solver(&model);
	
	//save
	model.save();
}
