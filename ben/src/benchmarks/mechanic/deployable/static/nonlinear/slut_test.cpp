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
#include "Analysis/Strategies/Types.h"
#include "Analysis/Solvers/Static_Nonlinear.h"

//ben
#include "benchmarks/mechanic/deployable.h"

//controls
const static bool open = true;
const static bool depl = false;
const static bool grav = false;
const static bool fric = false;
const static bool line = false;
const static bool alte = false;
const static bool sups = false;

//geometry
const static double L = 1.00e+00;
const static double h = 0.50e+00;
const static double H = 0.40e+00;

const static double r = 5.00e-02;
const static double s = 1.00e-02;
const static double q = 5.00e-02;

//joint
const static double mp = 5.00e+00;
const static double kt = 1.00e+03;
const static double kp = 0.00e+00;
const static double kr = 1.00e+05;
const static double ku = 1.00e+07;

//material
const static double p = 2.70e+03;
const static double E = 7.00e+10;

//cross section
const static double hs = 4.00e-02;
const static double ws = 1.00e-02;
const static double ts = 1.00e-03;

//analysis
const static double pr = 1.00e+00;
const static double dP = 1.00e+01;
const static double dp = 5.00e-01;
const static double pl = 1.00e+04;
const static double tl = 1.00e-04;

const static unsigned nx = 5;
const static unsigned ny = 5;
const static unsigned nr = 1;
const static unsigned na = 5;
const static unsigned ni = 20;
const static unsigned nm = 20;
const static unsigned ns = 1000;
const static unsigned nu = nx * ny;
const static unsigned nc = nx + ny + nu + 1;
const static unsigned ne = nx + ny + 2 * nu;

const static fea::analysis::strategies::type st1 = fea::analysis::strategies::type::control_load;
const static fea::analysis::strategies::type st2 = fea::analysis::strategies::type::minimal_norm;
//const static fea::analysis::strategies::type st2 = fea::analysis::strategies::type::control_state;
//const static fea::analysis::strategies::type st2 = fea::analysis::strategies::type::arc_length_cylindric;

//parameters
const static double D = M_SQRT2 * L;
const static double v = 2 * (M_SQRT2 - 1) * r;
const static double l = sqrt(h * h + pow(L - 2 * r, 2));
const static double e = sqrt(H * H + pow(D - 2 * r, 2)) / 2;
const static double x = (pow(D - 2 * r, 2) - v * v) / (2 * sqrt(4 * e * e - v * v) * l - 2 * h * H);

const static double d = x * l;
const static double b = (1 - x) * l;
const static double z = h - (1 - x) / x * H;
const static double c = sqrt(4 * e * e - v * v) - d;
const static double a = (1 - x) / x * c;

const static double ul = (D - 2 * M_SQRT2 * r) * nx;

const static fea::mesh::nodes::dof rx = fea::mesh::nodes::dof::rotation_x;
const static fea::mesh::nodes::dof ry = fea::mesh::nodes::dof::rotation_y;
const static fea::mesh::nodes::dof rz = fea::mesh::nodes::dof::rotation_z;
const static fea::mesh::nodes::dof ux = fea::mesh::nodes::dof::translation_x;
const static fea::mesh::nodes::dof uy = fea::mesh::nodes::dof::translation_y;
const static fea::mesh::nodes::dof uz = fea::mesh::nodes::dof::translation_z;

//data
static double p0[ny][nx][3], p1[ny][nx][3];
static double pa[ny][nx][4][2][3], pb[ny][nx][4][2][3], po[ny][nx][4][2][3];
static double pc[ny][nx][4][2][3], pd[ny][nx][4][7][3], pe[ny][nx][4][7][3];

//create
static void set_points(void)
{
	double t, f = s, xc, yc;
	for(unsigned i = 0; i < ny; i++)
	{
		f *= alte ? -1 : +1;
		for(unsigned j = 0; j < nx; j++)
		{
			xc = (D + q) * j;
			yc = (D + q) * i;
			f *= alte ? -1 : +1;
			p0[i][j][0] = p1[i][j][0] = xc;
			p0[i][j][1] = p1[i][j][1] = yc;
			p0[i][j][2] = open ? z : d - a;
			p1[i][j][2] = open ? h : d + b;
			for(unsigned k = 0; k < 4; k++)
			{
				t = (2 * k + 1) * M_PI / 4;
				pa[i][j][k][1][0] = xc + r * cos(t) + f * sin(t);
				pa[i][j][k][1][1] = yc + r * sin(t) - f * cos(t);
				pb[i][j][k][1][0] = xc + r * cos(t) - f * sin(t);
				pb[i][j][k][1][1] = yc + r * sin(t) + f * cos(t);
				pa[i][j][k][0][0] = pb[i][j][k][0][0] = xc + r * cos(t);
				pa[i][j][k][0][1] = pb[i][j][k][0][1] = yc + r * sin(t);
				pa[i][j][k][0][2] = pa[i][j][k][1][2] = open ? z : d - a;
				pb[i][j][k][0][2] = pb[i][j][k][1][2] = open ? h : d + b;
				po[i][j][k][0][2] = po[i][j][k][1][2] = open ? x * h : d;
				pc[i][j][k][0][2] = pc[i][j][k][1][2] = open ? H / 2 : (d + c) / 2;
				pd[i][j][k][4][0] = xc + (open ? L - r : r) * cos(t) + f * sin(t);
				pd[i][j][k][4][1] = yc + (open ? L - r : r) * sin(t) - f * cos(t);
				pe[i][j][k][4][0] = xc + (open ? L - r : r) * cos(t) - f * sin(t);
				pe[i][j][k][4][1] = yc + (open ? L - r : r) * sin(t) + f * cos(t);
				pd[i][j][k][0][0] = pe[i][j][k][0][0] = xc + (open ? L : 2 * r) * cos(t);
				pd[i][j][k][0][1] = pe[i][j][k][0][1] = yc + (open ? L : 2 * r) * sin(t);
				pd[i][j][k][1][0] = pe[i][j][k][1][0] = xc + (open ? L - r : r) * cos(t);
				pd[i][j][k][1][1] = pe[i][j][k][1][1] = yc + (open ? L - r : r) * sin(t);
				po[i][j][k][0][0] = xc + (open ? r + (1 - x) * (L - 2 * r) : r) * cos(t) + f * sin(t);
				po[i][j][k][0][1] = yc + (open ? r + (1 - x) * (L - 2 * r) : r) * sin(t) - f * cos(t);
				po[i][j][k][1][0] = xc + (open ? r + (1 - x) * (L - 2 * r) : r) * cos(t) - f * sin(t);
				po[i][j][k][1][1] = yc + (open ? r + (1 - x) * (L - 2 * r) : r) * sin(t) + f * cos(t);
				pc[i][j][k][0][0] = xc + ((open ? L : 2 * r) * cos(M_PI / 4) - f) * cos(t + M_PI / 4);
				pc[i][j][k][0][1] = yc + ((open ? L : 2 * r) * cos(M_PI / 4) - f) * sin(t + M_PI / 4);
				pc[i][j][k][1][0] = xc + ((open ? L : 2 * r) * cos(M_PI / 4) + f) * cos(t + M_PI / 4);
				pc[i][j][k][1][1] = yc + ((open ? L : 2 * r) * cos(M_PI / 4) + f) * sin(t + M_PI / 4);
				pd[i][j][k][2][0] = pe[i][j][k][2][0] = xc + (open ? L : 2 * r) * cos(t) + r * sin(t - M_PI / 4);
				pd[i][j][k][2][1] = pe[i][j][k][2][1] = yc + (open ? L : 2 * r) * sin(t) - r * cos(t - M_PI / 4);
				pd[i][j][k][3][0] = pe[i][j][k][3][0] = xc + (open ? L : 2 * r) * cos(t) - r * sin(t + M_PI / 4);
				pd[i][j][k][3][1] = pe[i][j][k][3][1] = yc + (open ? L : 2 * r) * sin(t) + r * cos(t + M_PI / 4);
				pd[i][j][k][5][0] = xc + (open ? L : 2 * r) * cos(t) + r * sin(t - M_PI / 4) + f * cos(t - M_PI / 4);
				pd[i][j][k][5][1] = yc + (open ? L : 2 * r) * sin(t) - r * cos(t - M_PI / 4) + f * sin(t - M_PI / 4);
				pd[i][j][k][6][0] = xc + (open ? L : 2 * r) * cos(t) - r * sin(t + M_PI / 4) - f * cos(t + M_PI / 4);
				pd[i][j][k][6][1] = yc + (open ? L : 2 * r) * sin(t) + r * cos(t + M_PI / 4) - f * sin(t + M_PI / 4);
				pe[i][j][k][5][0] = xc + (open ? L : 2 * r) * cos(t) + r * sin(t - M_PI / 4) - f * cos(t - M_PI / 4);
				pe[i][j][k][5][1] = yc + (open ? L : 2 * r) * sin(t) - r * cos(t - M_PI / 4) - f * sin(t - M_PI / 4);
				pe[i][j][k][6][0] = xc + (open ? L : 2 * r) * cos(t) - r * sin(t + M_PI / 4) + f * cos(t + M_PI / 4);
				pe[i][j][k][6][1] = yc + (open ? L : 2 * r) * sin(t) + r * cos(t + M_PI / 4) + f * sin(t + M_PI / 4);
				for(unsigned p = 0; p < 7; p++)
				{
					pe[i][j][k][p][2] = 0;
					pd[i][j][k][p][2] = open ? H : d + c;
				}
				
			}
		}
	}
}

static void create_nodes(fea::models::Model* model)
{
	/*
		k: diagonal
		0:				P0	(lower center hub middle)
		1:				P1	(upper center hub middle)
		22 * k +  2:	Pa	(lower center hub leg)
		22 * k +  3:	Pa	(lower center hub leg)
		22 * k +  4:	Pb	(upper center hub leg)
		22 * k +  5:	Pb	(upper center hub leg)
		22 * k +  6:	Po	(inner pin point)
		22 * k +  7:	Po	(inner pin point)
		22 * k +  8:	Pc	(outer pin point)
		22 * k +  9:	Pc	(outer pin point)
		22 * k + 10:	Pd	(upper side hub center)
		22 * k + 11:	Pd	(upper side hub leg)
		22 * k + 12:	Pd	(upper side hub leg)
		22 * k + 13:	Pd	(upper side hub leg)
		22 * k + 14:	Pd	(upper side hub leg)
		22 * k + 15:	Pd	(upper side hub leg)
		22 * k + 16:	Pd	(upper side hub leg)
		22 * k + 17:	Pe	(lower side hub center)
		22 * k + 18:	Pe	(lower side hub leg)
		22 * k + 19:	Pe	(lower side hub leg)
		22 * k + 20:	Pe	(lower side hub leg)
		22 * k + 21:	Pe	(lower side hub leg)
		22 * k + 22:	Pe	(lower side hub leg)
		22 * k + 23:	Pe	(lower side hub leg)
	*/
	for(unsigned i = 0; i < ny; i++)
	{
		for(unsigned j = 0; j < nx; j++)
		{
			model->mesh()->add_node(p0[i][j]);
			model->mesh()->add_node(p1[i][j]);
			for(unsigned k = 0; k < 4; k++)
			{
				model->mesh()->add_node(pa[i][j][k][0]);
				model->mesh()->add_node(pa[i][j][k][1]);
				model->mesh()->add_node(pb[i][j][k][0]);
				model->mesh()->add_node(pb[i][j][k][1]);
				model->mesh()->add_node(po[i][j][k][0]);
				model->mesh()->add_node(po[i][j][k][1]);
				model->mesh()->add_node(pc[i][j][k][0]);
				model->mesh()->add_node(pc[i][j][k][1]);
				model->mesh()->add_node(pd[i][j][k][0]);
				model->mesh()->add_node(pd[i][j][k][1]);
				model->mesh()->add_node(pd[i][j][k][2]);
				model->mesh()->add_node(pd[i][j][k][3]);
				model->mesh()->add_node(pd[i][j][k][4]);
				model->mesh()->add_node(pd[i][j][k][5]);
				model->mesh()->add_node(pd[i][j][k][6]);
				model->mesh()->add_node(pe[i][j][k][0]);
				model->mesh()->add_node(pe[i][j][k][1]);
				model->mesh()->add_node(pe[i][j][k][2]);
				model->mesh()->add_node(pe[i][j][k][3]);
				model->mesh()->add_node(pe[i][j][k][4]);
				model->mesh()->add_node(pe[i][j][k][5]);
				model->mesh()->add_node(pe[i][j][k][6]);
			}
		}
	}
}
static void create_cells(fea::models::Model* model)
{
	model->mesh()->add_cell(fea::mesh::cells::type::beam);
}
static void create_joints(fea::models::Model* model)
{
	for(unsigned i = 0; i < ny; i++)
	{
		for(unsigned j = 0; j < nx; j++)
		{
			for(unsigned k = 0; k < 4; k++)
			{
				const int n = 90 * (nx * i + j);
				const double t = (2 * k + 1) * M_PI / 4;
				const double s1[] = {+cos(t), +sin(t), 0};
				const double s2[] = {-sin(t), +cos(t), 0};
				const double u1[] = {+cos(t - M_PI / 4), +sin(t - M_PI / 4), 0};
				const double u2[] = {-sin(t - M_PI / 4), +cos(t - M_PI / 4), 0};
				const double e1[] = {+cos(t + M_PI / 4), +sin(t + M_PI / 4), 0};
				const double e2[] = {-sin(t + M_PI / 4), +cos(t + M_PI / 4), 0};
				const double* ha[] = {s2, s2, s2, e1, s2, u1, e1, s2, u1, e1};
				const double* ho[] = {s1, s1, s1, e2, s1, u2, e2, s1, u2, e2}; 
				model->mesh()->add_joint(fea::mesh::joints::type::hinge, {n + 22 * k +  2, n + 22 * k +  3});
				model->mesh()->add_joint(fea::mesh::joints::type::hinge, {n + 22 * k +  4, n + 22 * k +  5});
				model->mesh()->add_joint(fea::mesh::joints::type::hinge, {n + 22 * k +  6, n + 22 * k +  7});
				model->mesh()->add_joint(fea::mesh::joints::type::hinge, {n + 22 * k +  8, n + 22 * k +  9});
				model->mesh()->add_joint(fea::mesh::joints::type::hinge, {n + 22 * k + 11, n + 22 * k + 14});
				model->mesh()->add_joint(fea::mesh::joints::type::hinge, {n + 22 * k + 12, n + 22 * k + 15});
				model->mesh()->add_joint(fea::mesh::joints::type::hinge, {n + 22 * k + 13, n + 22 * k + 16});
				model->mesh()->add_joint(fea::mesh::joints::type::hinge, {n + 22 * k + 18, n + 22 * k + 21});
				model->mesh()->add_joint(fea::mesh::joints::type::hinge, {n + 22 * k + 19, n + 22 * k + 22});
				model->mesh()->add_joint(fea::mesh::joints::type::hinge, {n + 22 * k + 20, n + 22 * k + 23});
				for(unsigned p = 0; p < 10; p++)
				{
					((fea::mesh::joints::Hinge*) model->mesh()->joint(40 * (nx * i + j) + 10 * k + p))->axis(ha[p]);
					((fea::mesh::joints::Hinge*) model->mesh()->joint(40 * (nx * i + j) + 10 * k + p))->orientation(ho[p]);
					((fea::mesh::joints::Hinge*) model->mesh()->joint(40 * (nx * i + j) + 10 * k + p))->stiffness(0, 0, kr);
					((fea::mesh::joints::Hinge*) model->mesh()->joint(40 * (nx * i + j) + 10 * k + p))->stiffness(0, 1, kr);
					((fea::mesh::joints::Hinge*) model->mesh()->joint(40 * (nx * i + j) + 10 * k + p))->moment_yield(fric ? mp : 1);
					((fea::mesh::joints::Hinge*) model->mesh()->joint(40 * (nx * i + j) + 10 * k + p))->plastic_modulus(fric ? kp : 0);
					((fea::mesh::joints::Hinge*) model->mesh()->joint(40 * (nx * i + j) + 10 * k + p))->stiffness(0, 2, fric ? kt : 0);
				}
			}
		}
	}
}
static void create_plates(fea::models::Model* model)
{
	for(unsigned i = 0; i < ny; i++)
	{
		for(unsigned j = 1; j < nx; j++)
		{
			const unsigned p0 = 90 * (nx * i + j + 0) + 22 * 1 + 10;
			const unsigned p1 = 90 * (nx * i + j + 0) + 22 * 2 + 10;
			const unsigned p2 = 90 * (nx * i + j - 1) + 22 * 0 + 10;
			const unsigned p3 = 90 * (nx * i + j - 1) + 22 * 3 + 10;
			const unsigned p4 = 90 * (nx * i + j + 0) + 22 * 1 + 17;
			const unsigned p5 = 90 * (nx * i + j + 0) + 22 * 2 + 17;
			const unsigned p6 = 90 * (nx * i + j - 1) + 22 * 0 + 17;
			const unsigned p7 = 90 * (nx * i + j - 1) + 22 * 3 + 17;
			model->mesh()->add_element(fea::mesh::elements::type::beam3, {p0, p2});
			model->mesh()->add_element(fea::mesh::elements::type::beam3, {p1, p3});
			model->mesh()->add_element(fea::mesh::elements::type::beam3, {p4, p6});
			model->mesh()->add_element(fea::mesh::elements::type::beam3, {p5, p7});
			for(unsigned k = 0; k < 4; k++)
			{
				const unsigned e = 4 * ((nx - 1) * i + j - 1) + k;
				((fea::mesh::elements::Beam3*) model->mesh()->element(e))->orientation(0, 1, 0);
			}
		}
	}
	for(unsigned i = 0; i < nx; i++)
	{
		for(unsigned j = 1; j < ny; j++)
		{
			const unsigned p0 = 90 * (nx * (j + 0) + i) + 22 * 2 + 10;
			const unsigned p1 = 90 * (nx * (j + 0) + i) + 22 * 3 + 10;
			const unsigned p2 = 90 * (nx * (j - 1) + i) + 22 * 1 + 10;
			const unsigned p3 = 90 * (nx * (j - 1) + i) + 22 * 0 + 10;
			const unsigned p4 = 90 * (nx * (j + 0) + i) + 22 * 2 + 17;
			const unsigned p5 = 90 * (nx * (j + 0) + i) + 22 * 3 + 17;
			const unsigned p6 = 90 * (nx * (j - 1) + i) + 22 * 1 + 17;
			const unsigned p7 = 90 * (nx * (j - 1) + i) + 22 * 0 + 17;
			model->mesh()->add_element(fea::mesh::elements::type::beam3, {p0, p2});
			model->mesh()->add_element(fea::mesh::elements::type::beam3, {p1, p3});
			model->mesh()->add_element(fea::mesh::elements::type::beam3, {p4, p6});
			model->mesh()->add_element(fea::mesh::elements::type::beam3, {p5, p7});
			for(unsigned k = 0; k < 4; k++)
			{
				const unsigned e = 4 * (ny * (nx - 1) + (ny - 1) * i + j - 1) + k;
				((fea::mesh::elements::Beam3*) model->mesh()->element(e))->orientation(1, 0, 0);
			}
		}
	}
	for(unsigned i = 1; i < ny; i++)
	{
		for(unsigned j = 1; j < nx; j++)
		{
			const unsigned p0 = 90 * (nx * (i + 0) + j + 0) + 22 * 2 + 10;
			const unsigned p1 = 90 * (nx * (i + 0) + j - 1) + 22 * 3 + 10;
			const unsigned p2 = 90 * (nx * (i - 1) + j - 1) + 22 * 0 + 10;
			const unsigned p3 = 90 * (nx * (i - 1) + j + 0) + 22 * 1 + 10;
			const unsigned p4 = 90 * (nx * (i + 0) + j + 0) + 22 * 2 + 17;
			const unsigned p5 = 90 * (nx * (i + 0) + j - 1) + 22 * 3 + 17;
			const unsigned p6 = 90 * (nx * (i - 1) + j - 1) + 22 * 0 + 17;
			const unsigned p7 = 90 * (nx * (i - 1) + j + 0) + 22 * 1 + 17;
			model->mesh()->add_element(fea::mesh::elements::type::beam3, {p0, p2});
			model->mesh()->add_element(fea::mesh::elements::type::beam3, {p1, p3});
			model->mesh()->add_element(fea::mesh::elements::type::beam3, {p4, p6});
			model->mesh()->add_element(fea::mesh::elements::type::beam3, {p5, p7});
			for(unsigned k = 0; k < 4; k++)
			{
				const unsigned e = 4 * (ny * (nx - 1) + nx * (ny - 1) + (nx - 1) * (i - 1) + j - 1) + k;
				((fea::mesh::elements::Beam3*) model->mesh()->element(e))->orientation(mat::sign(bool(k % 2)) * M_SQRT2 / 2, M_SQRT2 / 2, 0);
			}
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
	/*
		k: diagonal
		16 * k +  0:	P0 -> Pa (lower center hub)
		16 * k +  1:	P1 -> Pb (upper center hub)
		16 * k +  2:	Pa -> Po (inner beam)
		16 * k +  3:	Pb -> Po (inner beam)
		16 * k +  4:	Pd -> Po (inner beam)
		16 * k +  5:	Pe -> Po (inner beam)
		16 * k +  6:	P0 -> Pa (outer beam)
		16 * k +  7:	P0 -> Pa (outer beam)
		16 * k +  8:	P0 -> Pa (lower side hub)
		16 * k +  9:	P0 -> Pa (lower side hub)
		16 * k + 10:	P0 -> Pa (lower side hub)
		16 * k + 11:	P0 -> Pa (upper side hub)
		16 * k + 12:	P0 -> Pa (upper side hub)
		16 * k + 13:	P0 -> Pa (upper side hub)
		16 * k + 14:	P0 -> Pa (outer beam)
		16 * k + 15:	P0 -> Pa (outer beam)
	*/
	for(unsigned i = 0; i < ny; i++)
	{
		for(unsigned j = 0; j < nx; j++)
		{
			for(unsigned k = 0; k < 4; k++)
			{
				const unsigned n = 90 * (nx * i + j);
				const double t = (2 * k + 1) * M_PI / 4;
				const unsigned m = k + 1 != 4 ? k + 1 : 0;
				const double s2[] = {-sin(t), +cos(t), 0};
				const double u1[] = {+cos(t - M_PI / 4), +sin(t - M_PI / 4), 0};
				const double e1[] = {+cos(t + M_PI / 4), +sin(t + M_PI / 4), 0};
				const double* bo[] = {s2, s2, s2, s2, s2, s2, e1, e1, s2, u1, e1, s2, u1, e1, e1, e1};
				model->mesh()->add_element(fea::mesh::elements::type::beam3, {n + 22 * k +  2, n + 0});
				model->mesh()->add_element(fea::mesh::elements::type::beam3, {n + 22 * k +  4, n + 1});
				model->mesh()->add_element(fea::mesh::elements::type::beam3, {n + 22 * k +  3, n + 22 * k +  6});
				model->mesh()->add_element(fea::mesh::elements::type::beam3, {n + 22 * k +  5, n + 22 * k +  7});
				model->mesh()->add_element(fea::mesh::elements::type::beam3, {n + 22 * k +  6, n + 22 * k + 14});
				model->mesh()->add_element(fea::mesh::elements::type::beam3, {n + 22 * k +  7, n + 22 * k + 21});
				model->mesh()->add_element(fea::mesh::elements::type::beam3, {n + 22 * k +  8, n + 22 * k + 16});
				model->mesh()->add_element(fea::mesh::elements::type::beam3, {n + 22 * k +  9, n + 22 * k + 23});
				model->mesh()->add_element(fea::mesh::elements::type::beam3, {n + 22 * k + 10, n + 22 * k + 11});
				model->mesh()->add_element(fea::mesh::elements::type::beam3, {n + 22 * k + 10, n + 22 * k + 12});
				model->mesh()->add_element(fea::mesh::elements::type::beam3, {n + 22 * k + 10, n + 22 * k + 13});
				model->mesh()->add_element(fea::mesh::elements::type::beam3, {n + 22 * k + 17, n + 22 * k + 18});
				model->mesh()->add_element(fea::mesh::elements::type::beam3, {n + 22 * k + 17, n + 22 * k + 19});
				model->mesh()->add_element(fea::mesh::elements::type::beam3, {n + 22 * k + 17, n + 22 * k + 20});
				model->mesh()->add_element(fea::mesh::elements::type::beam3, {n + 22 * k +  8, n + 22 * m + 22});
				model->mesh()->add_element(fea::mesh::elements::type::beam3, {n + 22 * k +  9, n + 22 * m + 15});
				for(unsigned p = 0; p < 16; p++)
				{
					const unsigned e = 4 * (3 * nx * ny - 2 * (nx + ny) + 1) + p;
					((fea::mesh::elements::Beam3*) model->mesh()->element(64 * (nx * i + j) + 16 * k + e))->orientation(bo[p]);
				}
			}
		}
	}
}
static void refine_elements(fea::models::Model* model)
{
	for(unsigned i = 0; i < ny; i++)
	{
		for(unsigned j = 0; j < nx; j++)
		{
			for(unsigned k = 0; k < 4; k++)
			{
				const unsigned e = 4 * (3 * nx * ny - 2 * (nx + ny) + 1);
				fea::mesh::cells::Line::refine(64 * (nx * i + j) + 16 * k + e +  2, nr);
				fea::mesh::cells::Line::refine(64 * (nx * i + j) + 16 * k + e +  3, nr);
				fea::mesh::cells::Line::refine(64 * (nx * i + j) + 16 * k + e +  4, nr);
				fea::mesh::cells::Line::refine(64 * (nx * i + j) + 16 * k + e +  5, nr);
				fea::mesh::cells::Line::refine(64 * (nx * i + j) + 16 * k + e +  6, nr);
				fea::mesh::cells::Line::refine(64 * (nx * i + j) + 16 * k + e +  7, nr);
				fea::mesh::cells::Line::refine(64 * (nx * i + j) + 16 * k + e + 14, nr);
				fea::mesh::cells::Line::refine(64 * (nx * i + j) + 16 * k + e + 15, nr);
			}
		}
	}
}
static void create_materials(fea::models::Model* model)
{
	model->mesh()->add_material(fea::mesh::materials::type::steel);
	((fea::mesh::materials::Steel*) model->mesh()->material(0))->specific_mass(p);
	((fea::mesh::materials::Steel*) model->mesh()->material(0))->elastic_modulus(E);
}

static void create_loads(fea::models::Model* model)
{
	//load cases
	model->boundary()->add_load_case("Deployment");
	model->boundary()->add_self_weight("Gravity", fea::mesh::nodes::dof::translation_z);
	for(unsigned i = 0; i < nx; i++)
	{
		model->boundary()->load_case(0)->add_load_node(90 * ((ny - 1) * nx + i) + 22 * 0 + 10, uy, -mat::sign(open) * pr);
		model->boundary()->load_case(0)->add_load_node(90 * ((ny - 1) * nx + i) + 22 * 1 + 10, uy, -mat::sign(open) * pr);
		model->boundary()->load_case(0)->add_load_node(90 * ((ny - 1) * nx + i) + 22 * 0 + 17, uy, -mat::sign(open) * pr);
		model->boundary()->load_case(0)->add_load_node(90 * ((ny - 1) * nx + i) + 22 * 1 + 17, uy, -mat::sign(open) * pr);
	}
	for(unsigned i = 0; i < ny; i++)
	{
//		model->boundary()->load_case(0)->add_load_node(90 * (nx * i + nx - 1) + 22 * 0 + 10, ux, -mat::sign(open) * pr);
//		model->boundary()->load_case(0)->add_load_node(90 * (nx * i + nx - 1) + 22 * 3 + 10, ux, -mat::sign(open) * pr);
//		model->boundary()->load_case(0)->add_load_node(90 * (nx * i + nx - 1) + 22 * 0 + 17, ux, -mat::sign(open) * pr);
//		model->boundary()->load_case(0)->add_load_node(90 * (nx * i + nx - 1) + 22 * 3 + 17, ux, -mat::sign(open) * pr);
	}
}
static void create_supports(fea::models::Model* model)
{
	for(unsigned i = 0; i < nx; i++)
	{
		model->boundary()->add_support(90 * i + 22 * 2 + 17, uy);
		model->boundary()->add_support(90 * i + 22 * 3 + 17, uy);
		model->boundary()->add_support(90 * i + 22 * 2 + 17, uz);
		model->boundary()->add_support(90 * i + 22 * 3 + 17, uz);
		if(sups)
		{
			model->boundary()->add_support(90 * ((ny - 1) * nx + i) + 22 * 2 + 10, uy);
			model->boundary()->add_support(90 * ((ny - 1) * nx + i) + 22 * 3 + 10, uy);
		}
	}
	for(unsigned i = 0; i < ny; i++)
	{
		model->boundary()->add_support(90 * nx * i + 22 * 1 + 17, ux);
		model->boundary()->add_support(90 * nx * i + 22 * 2 + 17, ux);
		if(sups)
		{
			model->boundary()->add_support(90 * nx * i + 22 * 1 + 10, ux);
			model->boundary()->add_support(90 * nx * i + 22 * 2 + 10, ux);
		}
	}
//	for(unsigned i = 0; i < ny; i++)
//	{
//		for(unsigned j = 0; j < nx; j++)
//		{
//			for(unsigned k = 0; k < 4; k++)
//			{
//				model->boundary()->add_support(90 * (nx * i + j) + 22 * k + 17, uz);
//			}
//		}
//	}
}

static void create_solver(fea::models::Model* model)
{
	fea::mesh::elements::Mechanic::geometric(true);
	model->analysis()->solver(fea::analysis::solvers::type::static_nonlinear);
	model->analysis()->solver()->watch_dof(90 * (nx - 1) + 22 * 3 + 17, ux);

	if(grav)
	{
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model->analysis()->solver())->load_case(1);
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model->analysis()->solver())->step_max(100);
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model->analysis()->solver())->load_max(1e0);
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model->analysis()->solver())->strategy(st1);
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model->analysis()->solver())->load_predictor(5e-2);
		model->analysis()->solve();
	}
	
	if(depl)
	{
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model->analysis()->solver())->dead(grav);
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model->analysis()->solver())->dof_min(-ul);
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model->analysis()->solver())->dead_case(1);
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model->analysis()->solver())->load_case(0);
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model->analysis()->solver())->step_max(ns);
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model->analysis()->solver())->load_max(pl);
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model->analysis()->solver())->strategy(st2);
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model->analysis()->solver())->tolerance(tl);
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model->analysis()->solver())->attempt_max(na);
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model->analysis()->solver())->load_adjust(true);
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model->analysis()->solver())->iteration_max(nm);
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model->analysis()->solver())->load_predictor(dp);
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model->analysis()->solver())->iteration_desired(ni);
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model->analysis()->solver())->load_increment_max(dP);
		model->analysis()->solve(grav);
	}
}

//model
void tests::deployable::static_nonlinear::slut_test(void)
{
	//model
	char name[200], path[200];
	sprintf(name, "plot");
//	sprintf(name, "%02d", nx);
//	sprintf(name, "mp_%d", unsigned(mp));
	sprintf(path, "benchmarks/deployable/static/nonlinear/slut test/models/%s/%s", line  ? "line" : "wall", alte ? "alternate" : "uniform");
	fea::models::Model model(name, path);
	
	//mesh
	set_points();
	create_nodes(&model);
	create_cells(&model);
	create_joints(&model);
	create_plates(&model);
	create_sections(&model);
	create_elements(&model);
	refine_elements(&model);
	create_materials(&model);
	
	//boundary
	create_loads(&model);
	create_supports(&model);

	//analysis
	create_solver(&model);
	
	//save
	model.save();
}
