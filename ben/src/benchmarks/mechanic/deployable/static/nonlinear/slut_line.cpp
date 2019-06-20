//std
#include <cmath>
#include <ctime>

//mat
#include "misc/defs.h"
#include "misc/util.h"
#include "linear/dense.h"

//fea
#include "Models/Model.h"

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
const static bool depl = true;
const static bool grav = true;
const static bool alte = false;
const static bool sups = false;
const static bool fric = false;
const static bool vary = false;
const static bool unlo = false;

//units
const static int n = 5;

//geometry
const static double h = 0.50e+00;
const static double H = 0.40e+00;
const static double S = 1 * n * M_SQRT2;

const static double r = 5.00e-02;
const static double s = 1.00e-02;

//joint
const static double mp = 5.00e+00;
const static double kt = 1.00e+03;
const static double kp = 1.00e-03;
const static double kr = 1.00e+02;
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
const static double d0 = 5.00e-01;
const static double dp = 1.00e-01;
const static double dP = 1.00e+05;
const static double pl = 1.00e+06;
const static double tl = 1.00e-04;

const static unsigned st = 1;
const static unsigned nr = 1;
const static unsigned nx = n;
const static unsigned ny = 1;
const static unsigned ni = 8;
const static unsigned na = 5;
const static unsigned nm = 15;
const static unsigned ns = 1500;
const static unsigned nu = nx * ny;
const static unsigned nc = nx + ny + nu + 1;
const static unsigned ne = nx + ny + 2 * nu;

//parameters
const static double D = S / n;
const static double L = M_SQRT2 / 2 * D;
const static double v = 2 * (M_SQRT2 - 1) * r;
const static double l = sqrt(h * h + pow(L - 2 * r, 2));
const static double e = sqrt(H * H + pow(D - 2 * r, 2)) / 2;
const static double x = ((D - 2 * r) * (D - 2 * r) - v * v) / (2 * sqrt(4 * e * e - v * v) * l - 2 * h * H);

const static double d = x * l;
const static double b = (1 - x) * l;
const static double z = h - (1 - x) / x * H;
const static double c = sqrt(4 * e * e - v * v) - d;
const static double a = (1 - x) / x * c;

const static double ul = (D - 2 * M_SQRT2 * r) * n;
const static double dc = open ? D : 2 * sqrt(2) * r;

const static fea::mesh::nodes::dof rx = fea::mesh::nodes::dof::rotation_x;
const static fea::mesh::nodes::dof ry = fea::mesh::nodes::dof::rotation_y;
const static fea::mesh::nodes::dof rz = fea::mesh::nodes::dof::rotation_z;
const static fea::mesh::nodes::dof ux = fea::mesh::nodes::dof::translation_x;
const static fea::mesh::nodes::dof uy = fea::mesh::nodes::dof::translation_y;
const static fea::mesh::nodes::dof uz = fea::mesh::nodes::dof::translation_z;

//data
static double p0[nu][3], p1[nu][3];
static double pf[nc][2][3], pc[ne][10][3];
static double pa[nu][4][2][3], pb[nu][4][2][3];
static double po[nu][4][2][3], pd[nu][4][2][3], pe[nu][4][2][3];

//create
static void set_points(void)
{
	//inner
	for(unsigned i = 0, u = 0; i < ny; i++)
	{
		for(unsigned j = 0; j < nx; j++, u++)
		{
			p0[u][2] = open ? z : d - a;
			p1[u][2] = open ? h : d + b;
			p0[u][0] = p1[u][0] = dc * j;
			p0[u][1] = p1[u][1] = dc * i;
			for(unsigned k = 0; k < 4; k++)
			{
				double t = (2 * k + 1) * M_PI / 4;
				double f = alte && j % 2 ? -s : +s;
				pe[u][k][0][2] = pe[u][k][1][2] = 0;
				pa[u][k][1][0] = dc * j + r * cos(t) + f * sin(t);
				pa[u][k][1][1] = dc * i + r * sin(t) - f * cos(t);
				pb[u][k][1][0] = dc * j + r * cos(t) - f * sin(t);
				pb[u][k][1][1] = dc * i + r * sin(t) + f * cos(t);
				pa[u][k][0][2] = pa[u][k][1][2] = open ? z : d - a;
				pb[u][k][0][2] = pb[u][k][1][2] = open ? h : d + b;
				pd[u][k][0][2] = pd[u][k][1][2] = open ? H : d + c;
				po[u][k][0][2] = po[u][k][1][2] = open ? x * h : d;
				pa[u][k][0][0] = pb[u][k][0][0] = dc * j + r * cos(t);
				pa[u][k][0][1] = pb[u][k][0][1] = dc * i + r * sin(t);
				pd[u][k][1][0] = dc * j + (open ? L - r : r) * cos(t) + f * sin(t);
				pd[u][k][1][1] = dc * i + (open ? L - r : r) * sin(t) - f * cos(t);
				pe[u][k][1][0] = dc * j + (open ? L - r : r) * cos(t) - f * sin(t);
				pe[u][k][1][1] = dc * i + (open ? L - r : r) * sin(t) + f * cos(t);
				pd[u][k][0][0] = pe[u][k][0][0] = dc * j + (open ? L - r : r) * cos(t);
				pd[u][k][0][1] = pe[u][k][0][1] = dc * i + (open ? L - r : r) * sin(t);
				po[u][k][0][0] = dc * j + (open ? r + (1 - x) * (L - 2 * r) : r) * cos(t) + f * sin(t);
				po[u][k][0][1] = dc * i + (open ? r + (1 - x) * (L - 2 * r) : r) * sin(t) - f * cos(t);
				po[u][k][1][0] = dc * j + (open ? r + (1 - x) * (L - 2 * r) : r) * cos(t) - f * sin(t);
				po[u][k][1][1] = dc * i + (open ? r + (1 - x) * (L - 2 * r) : r) * sin(t) + f * cos(t);
			}
		}
	}
	//edge
	double q = open ? H : d + c;
	for(unsigned i = 0, e = 0; i <= ny; i++)
	{
		for(unsigned j = 0; j < nx; j++, e++)
		{
			pc[e][0][2] = pc[e][1][2] = q / 2;
			pc[e][0][0] = pc[e][1][0] = dc * j;
			pc[e][2][2] = pc[e][3][2] = pc[e][4][2] = pc[e][5][2] = 0;
			pc[e][6][2] = pc[e][7][2] = pc[e][8][2] = pc[e][9][2] = q;
			pc[e][0][1] = pc[e][3][1] = pc[e][9][1] = dc * i - dc / 2 - s;
			pc[e][1][1] = pc[e][5][1] = pc[e][7][1] = dc * i - dc / 2 + s;
			pc[e][2][1] = pc[e][4][1] = pc[e][6][1] = pc[e][8][1] = dc * i - dc / 2;
			pc[e][2][0] = pc[e][3][0] = pc[e][6][0] = pc[e][7][0] = dc * j - dc / 2 + r;
			pc[e][4][0] = pc[e][5][0] = pc[e][8][0] = pc[e][9][0] = dc * j + dc / 2 - r;
		}
	}
	for(unsigned j = 0, e = nx * (ny + 1); j <= nx; j++)
	{
		for(unsigned i = 0; i < ny; i++, e++)
		{
			pc[e][0][2] = pc[e][1][2] = q / 2;
			pc[e][0][1] = pc[e][1][1] = dc * i;
			pc[e][2][2] = pc[e][3][2] = pc[e][4][2] = pc[e][5][2] = 0;
			pc[e][6][2] = pc[e][7][2] = pc[e][8][2] = pc[e][9][2] = q;
			pc[e][0][0] = pc[e][3][0] = pc[e][9][0] = dc * j - dc / 2 + s;
			pc[e][1][0] = pc[e][5][0] = pc[e][7][0] = dc * j - dc / 2 - s;
			pc[e][2][0] = pc[e][4][0] = pc[e][6][0] = pc[e][8][0] = dc * j - dc / 2;
			pc[e][2][1] = pc[e][3][1] = pc[e][6][1] = pc[e][7][1] = dc * i - dc / 2 + r;
			pc[e][4][1] = pc[e][5][1] = pc[e][8][1] = pc[e][9][1] = dc * i + dc / 2 - r;
		}
	}
	//corner
	for(unsigned i = 0, n = 0; i <= ny; i++)
	{
		for(unsigned j = 0; j <= nx; j++, n++)
		{
			pf[n][0][2] = 0;
			pf[n][1][2] = open ? H : d + c;
			pf[n][0][0] = pf[n][1][0] = dc * j - dc / 2;
			pf[n][0][1] = pf[n][1][1] = dc * i - dc / 2;
		}
	}
}
static void check_points(void)
{
	//Span
	printf("Span:\t%+.2e\n", S);
	printf("Disp:\t%+.2e\n", ul);
	
	//mass
	const double A = 2 * (ws + hs - 2 * ts) * ts;
	printf("Mass:\t%+.2e\n", p * A * (4 * nu * (4 * r + a + b + c + d) + 4 * ne * (e + r)));
	
	//check
	bool test = true;
	const double tol = 1e-5;
	for(unsigned i = 0; i < ny; i++)
	{
		for(unsigned j = 0; j < nx; j++)
		{
			for(unsigned k = 0; k < 4; k++)
			{
				const unsigned u = j + nx * i;
				const unsigned n[] = {
					(nx + 1) * (i + 1) + j + 1,
					(nx + 1) * (i + 1) + j + 0,
					(nx + 1) * (i + 0) + j + 0,
					(nx + 1) * (i + 0) + j + 1
				};
				test = test && fabs(mat::norm(pa[u][k][0], p0[u], 3) - r) < tol * L;
				test = test && fabs(mat::norm(pb[u][k][0], p1[u], 3) - r) < tol * L;
				test = test && fabs(mat::norm(pa[u][k][1], po[u][k][0], 3) - a) < tol * L;
				test = test && fabs(mat::norm(pb[u][k][1], po[u][k][1], 3) - b) < tol * L;
				test = test && fabs(mat::norm(pd[u][k][1], po[u][k][0], 3) - c) < tol * L;
				test = test && fabs(mat::norm(pe[u][k][1], po[u][k][1], 3) - d) < tol * L;
				test = test && fabs(mat::norm(pa[u][k][0], pa[u][k][1], 3) - s) < tol * L;
				test = test && fabs(mat::norm(pb[u][k][0], pb[u][k][1], 3) - s) < tol * L;
				test = test && fabs(mat::norm(pd[u][k][0], pd[u][k][1], 3) - s) < tol * L;
				test = test && fabs(mat::norm(pe[u][k][0], pe[u][k][1], 3) - s) < tol * L;
				test = test && fabs(mat::norm(pd[u][k][0], pf[n[k]][1], 3) - r) < tol * L;
				test = test && fabs(mat::norm(pe[u][k][0], pf[n[k]][0], 3) - r) < tol * L;
				test = test && fabs(mat::norm(po[u][k][0], po[u][k][1], 3) - 2 * s) < tol * L;
			}
		}
	}
	for(unsigned i = 0; i <= ny; i++)
	{
		for(unsigned j = 0; j < nx; j++)
		{
			const unsigned k = j + nx * i;
			const unsigned p = (nx + 1) * i + j + 0;
			const unsigned q = (nx + 1) * i + j + 1;
			test = test && fabs(mat::norm(pc[k][2], pc[k][3], 3) - s) < tol * L;
			test = test && fabs(mat::norm(pc[k][4], pc[k][5], 3) - s) < tol * L;
			test = test && fabs(mat::norm(pc[k][6], pc[k][7], 3) - s) < tol * L;
			test = test && fabs(mat::norm(pc[k][8], pc[k][9], 3) - s) < tol * L;
			test = test && fabs(mat::norm(pc[k][3], pc[k][0], 3) - e) < tol * L;
			test = test && fabs(mat::norm(pc[k][9], pc[k][0], 3) - e) < tol * L;
			test = test && fabs(mat::norm(pc[k][5], pc[k][1], 3) - e) < tol * L;
			test = test && fabs(mat::norm(pc[k][7], pc[k][1], 3) - e) < tol * L;
			test = test && fabs(mat::norm(pc[k][2], pf[p][0], 3) - r) < tol * L;
			test = test && fabs(mat::norm(pc[k][4], pf[q][0], 3) - r) < tol * L;
			test = test && fabs(mat::norm(pc[k][6], pf[p][1], 3) - r) < tol * L;
			test = test && fabs(mat::norm(pc[k][8], pf[q][1], 3) - r) < tol * L;
			test = test && fabs(mat::norm(pc[k][0], pc[k][1], 3) - 2 * s) < tol * L;
		}
	}
	const unsigned w = nx * (ny + 1);
	for(unsigned j = 0; j <= nx; j++)
	{
		for(unsigned i = 0; i < ny; i++)
		{
			const unsigned k = w + i + ny * j;
			const unsigned p = (nx + 1) * (i + 0) + j;
			const unsigned q = (nx + 1) * (i + 1) + j;
			test = test && fabs(mat::norm(pc[k][2], pc[k][3], 3) - s) < tol * L;
			test = test && fabs(mat::norm(pc[k][4], pc[k][5], 3) - s) < tol * L;
			test = test && fabs(mat::norm(pc[k][6], pc[k][7], 3) - s) < tol * L;
			test = test && fabs(mat::norm(pc[k][8], pc[k][9], 3) - s) < tol * L;
			test = test && fabs(mat::norm(pc[k][3], pc[k][0], 3) - e) < tol * L;
			test = test && fabs(mat::norm(pc[k][9], pc[k][0], 3) - e) < tol * L;
			test = test && fabs(mat::norm(pc[k][5], pc[k][1], 3) - e) < tol * L;
			test = test && fabs(mat::norm(pc[k][7], pc[k][1], 3) - e) < tol * L;
			test = test && fabs(mat::norm(pc[k][2], pf[p][0], 3) - r) < tol * L;
			test = test && fabs(mat::norm(pc[k][4], pf[q][0], 3) - r) < tol * L;
			test = test && fabs(mat::norm(pc[k][6], pf[p][1], 3) - r) < tol * L;
			test = test && fabs(mat::norm(pc[k][8], pf[q][1], 3) - r) < tol * L;
			test = test && fabs(mat::norm(pc[k][0], pc[k][1], 3) - 2 * s) < tol * L;
		}
	}
	printf("Sizes:\t%s!\n", test ? "ok" : "not ok");
}

static void create_nodes(fea::models::Model* model)
{
	//inner
	for(unsigned i = 0, u = 0; i < ny; i++)
	{
		for(unsigned j = 0; j < nx; j++, u++)
		{
			model->mesh()->add_node(p0[u]);
			model->mesh()->add_node(p1[u]);
			for(unsigned k = 0; k < 4; k++)
			{
				model->mesh()->add_node(pa[u][k][0]);
				model->mesh()->add_node(pa[u][k][1]);
				model->mesh()->add_node(pb[u][k][0]);
				model->mesh()->add_node(pb[u][k][1]);
				model->mesh()->add_node(po[u][k][0]);
				model->mesh()->add_node(po[u][k][1]);
				model->mesh()->add_node(pd[u][k][0]);
				model->mesh()->add_node(pd[u][k][1]);
				model->mesh()->add_node(pe[u][k][0]);
				model->mesh()->add_node(pe[u][k][1]);
			}
		}
	}
	//edge
	for(unsigned i = 0, e = 0; i <= ny; i++)
	{
		for(unsigned j = 0; j < nx; j++, e++)
		{
			model->mesh()->add_node(pc[e][0]);
			model->mesh()->add_node(pc[e][1]);
			model->mesh()->add_node(pc[e][2]);
			model->mesh()->add_node(pc[e][3]);
			model->mesh()->add_node(pc[e][4]);
			model->mesh()->add_node(pc[e][5]);
			model->mesh()->add_node(pc[e][6]);
			model->mesh()->add_node(pc[e][7]);
			model->mesh()->add_node(pc[e][8]);
			model->mesh()->add_node(pc[e][9]);
		}
	}
	for(unsigned j = 0, e = nx * (ny + 1); j <= nx; j++)
	{
		for(unsigned i = 0; i < ny; i++, e++)
		{
			model->mesh()->add_node(pc[e][0]);
			model->mesh()->add_node(pc[e][1]);
			model->mesh()->add_node(pc[e][2]);
			model->mesh()->add_node(pc[e][3]);
			model->mesh()->add_node(pc[e][4]);
			model->mesh()->add_node(pc[e][5]);
			model->mesh()->add_node(pc[e][6]);
			model->mesh()->add_node(pc[e][7]);
			model->mesh()->add_node(pc[e][8]);
			model->mesh()->add_node(pc[e][9]);
		}
	}
	//corner
	for(unsigned i = 0, c = 0; i <= ny; i++)
	{
		for(unsigned j = 0; j <= nx; j++, c++)
		{
			model->mesh()->add_node(pf[c][0]);
			model->mesh()->add_node(pf[c][1]);
		}
	}
}
static void create_cells(fea::models::Model* model)
{
	model->mesh()->add_cell(fea::mesh::cells::type::beam);
}
static void create_joints(fea::models::Model* model)
{
	//inner
	srand(time(nullptr));
	for(unsigned i = 0; i < ny; i++)
	{
		for(unsigned j = 0; j < nx; j++)
		{
			for(unsigned k = 0; k < 4; k++)
			{
				const unsigned p = 42 * (nx * i + j);
				const double t = (2 * k + 1) * M_PI / 4;
				const double a[] = {-sin(t), +cos(t), 0};
				const double h[] = {+cos(t), +sin(t), 0};
				const unsigned c[] = {
					42 * nu + 10 * ne + 2 * ((nx + 1) * (i + 1) + j + 1),
					42 * nu + 10 * ne + 2 * ((nx + 1) * (i + 1) + j + 0),
					42 * nu + 10 * ne + 2 * ((nx + 1) * (i + 0) + j + 0),
					42 * nu + 10 * ne + 2 * ((nx + 1) * (i + 0) + j + 1)
				};
				model->mesh()->add_joint(fea::mesh::joints::type::hinge, {p + 10 * k +  6, p + 10 * k +  7});
				model->mesh()->add_joint(fea::mesh::joints::type::hinge, {r == 0 ? p + 0 : p + 10 * k +  2, p + 10 * k +  3});
				model->mesh()->add_joint(fea::mesh::joints::type::hinge, {r == 0 ? p + 1 : p + 10 * k +  4, p + 10 * k +  5});
				model->mesh()->add_joint(fea::mesh::joints::type::hinge, {r == 0 ? c[k] + 1 : p + 10 * k +  8, p + 10 * k +  9});
				model->mesh()->add_joint(fea::mesh::joints::type::hinge, {r == 0 ? c[k] + 0 : p + 10 * k + 10, p + 10 * k + 11});
				for(unsigned q = 0; q < 5; q++)
				{
					const unsigned c = 5 * (4 * (nx * i + j) + k) + q;
					((fea::mesh::joints::Hinge*) model->mesh()->joint(c))->axis(a);
					((fea::mesh::joints::Hinge*) model->mesh()->joint(c))->orientation(h);
					((fea::mesh::joints::Hinge*) model->mesh()->joint(c))->stiffness(0, 0, kr);
					((fea::mesh::joints::Hinge*) model->mesh()->joint(c))->stiffness(0, 1, kr);
					((fea::mesh::joints::Hinge*) model->mesh()->joint(c))->stiffness(1, 0, ku);
					((fea::mesh::joints::Hinge*) model->mesh()->joint(c))->stiffness(1, 1, ku);
					((fea::mesh::joints::Hinge*) model->mesh()->joint(c))->stiffness(1, 2, ku);
					((fea::mesh::joints::Hinge*) model->mesh()->joint(c))->stiffness(0, 2, fric ? kt : 0);
					((fea::mesh::joints::Hinge*) model->mesh()->joint(c))->moment_yield(fric ? vary ? mat::randu(0, mp) : mp : 1);
				}
			}
		}
	}
	//edge
	for(unsigned i = 0; i <= ny; i++)
	{
		for(unsigned j = 0; j < nx; j++)
		{
			const double a[] = {0, 1, 0};
			const double h[] = {1, 0, 0};
			const unsigned p = 42 * nu + 10 * (nx * i + j);
			const unsigned c[] = {
				42 * nu + 10 * ne + 2 * ((nx + 1) * i + j + 0) + 0,
				42 * nu + 10 * ne + 2 * ((nx + 1) * i + j + 1) + 0,
				42 * nu + 10 * ne + 2 * ((nx + 1) * i + j + 0) + 1,
				42 * nu + 10 * ne + 2 * ((nx + 1) * i + j + 1) + 1
			};
			model->mesh()->add_joint(fea::mesh::joints::type::hinge, {p + 0, p + 1});
			model->mesh()->add_joint(fea::mesh::joints::type::hinge, {r == 0 ? c[0] : p + 2, p + 3});
			model->mesh()->add_joint(fea::mesh::joints::type::hinge, {r == 0 ? c[1] : p + 4, p + 5});
			model->mesh()->add_joint(fea::mesh::joints::type::hinge, {r == 0 ? c[2] : p + 6, p + 7});
			model->mesh()->add_joint(fea::mesh::joints::type::hinge, {r == 0 ? c[3] : p + 8, p + 9});
			for(unsigned q = 0; q < 5; q++)
			{
				const unsigned c = 20 * nu + 5 * (nx * i + j) + q;
				((fea::mesh::joints::Hinge*) model->mesh()->joint(c))->axis(a);
				((fea::mesh::joints::Hinge*) model->mesh()->joint(c))->orientation(h);
				((fea::mesh::joints::Hinge*) model->mesh()->joint(c))->stiffness(0, 0, kr);
				((fea::mesh::joints::Hinge*) model->mesh()->joint(c))->stiffness(0, 1, kr);
				((fea::mesh::joints::Hinge*) model->mesh()->joint(c))->stiffness(0, 2, fric ? kt : 0);
				((fea::mesh::joints::Hinge*) model->mesh()->joint(c))->moment_yield(fric ? vary ? mat::randu(0, mp) : mp : 1);
			}
		}
	}
	for(unsigned j = 0; j <= nx; j++)
	{
		for(unsigned i = 0; i < ny; i++)
		{
			const double a[] = {1, 0, 0};
			const double h[] = {0, 1, 0};
			const unsigned p = 42 * nu + 10 * nx * (ny + 1) + 10 * (ny * j + i);
			const unsigned c[] = {
				42 * nu + 10 * ne + 2 * ((nx + 1) * (i + 0) + j) + 0,
				42 * nu + 10 * ne + 2 * ((nx + 1) * (i + 1) + j) + 0,
				42 * nu + 10 * ne + 2 * ((nx + 1) * (i + 0) + j) + 1,
				42 * nu + 10 * ne + 2 * ((nx + 1) * (i + 1) + j) + 1
			};
			model->mesh()->add_joint(fea::mesh::joints::type::hinge, {p + 0, p + 1});
			model->mesh()->add_joint(fea::mesh::joints::type::hinge, {r == 0 ? c[0] : p + 2, p + 3});
			model->mesh()->add_joint(fea::mesh::joints::type::hinge, {r == 0 ? c[1] : p + 4, p + 5});
			model->mesh()->add_joint(fea::mesh::joints::type::hinge, {r == 0 ? c[2] : p + 6, p + 7});
			model->mesh()->add_joint(fea::mesh::joints::type::hinge, {r == 0 ? c[3] : p + 8, p + 9});
			for(unsigned q = 0; q < 5; q++)
			{
				const unsigned c = 20 * nu + 5 * (ny + 1) * nx + 5 * (ny * j + i) + q;
				((fea::mesh::joints::Hinge*) model->mesh()->joint(c))->axis(a);
				((fea::mesh::joints::Hinge*) model->mesh()->joint(c))->orientation(h);
				((fea::mesh::joints::Hinge*) model->mesh()->joint(c))->stiffness(0, 0, kr);
				((fea::mesh::joints::Hinge*) model->mesh()->joint(c))->stiffness(0, 1, kr);
				((fea::mesh::joints::Hinge*) model->mesh()->joint(c))->stiffness(0, 2, fric ? kt : 0);
				((fea::mesh::joints::Hinge*) model->mesh()->joint(c))->moment_yield(fric ? vary ? mat::randu(0, mp) : mp : 1);
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
	//inner
	const unsigned nd = r ? 8 : 4;
	for(unsigned i = 0; i < ny; i++)
	{
		for(unsigned j = 0; j < nx; j++)
		{
			for(unsigned k = 0; k < 4; k++)
			{
				const unsigned p = 42 * (nx * i + j);
				const unsigned c[] = {
					42 * nu + 10 * ne + 2 * ((nx + 1) * (i + 1) + j + 1),
					42 * nu + 10 * ne + 2 * ((nx + 1) * (i + 1) + j + 0),
					42 * nu + 10 * ne + 2 * ((nx + 1) * (i + 0) + j + 0),
					42 * nu + 10 * ne + 2 * ((nx + 1) * (i + 0) + j + 1)
				};
				//inner
				model->mesh()->add_element(fea::mesh::elements::type::beam3, {p + 10 * k +  6, p + 10 * k +  3});
				model->mesh()->add_element(fea::mesh::elements::type::beam3, {p + 10 * k +  6, p + 10 * k +  9});
				model->mesh()->add_element(fea::mesh::elements::type::beam3, {p + 10 * k +  7, p + 10 * k +  5});
				model->mesh()->add_element(fea::mesh::elements::type::beam3, {p + 10 * k +  7, p + 10 * k + 11});
				//hinge
				if(r != 0)
				{
					model->mesh()->add_element(fea::mesh::elements::type::beam3, {p + 10 * k +  2, p + 0});
					model->mesh()->add_element(fea::mesh::elements::type::beam3, {p + 10 * k +  4, p + 1});
					model->mesh()->add_element(fea::mesh::elements::type::beam3, {p + 10 * k +  8, c[k] + 1});
					model->mesh()->add_element(fea::mesh::elements::type::beam3, {p + 10 * k + 10, c[k] + 0});
				}
				//orientation
				for(unsigned q = 0; q < nd; q++)
				{
					const double t = (2 * k + 1) * M_PI / 4;
					const unsigned p = 4 * nd * (nx * i + j) + nd * k + q;
					((fea::mesh::elements::Beam3*) model->mesh()->element(p))->orientation(-sin(t), +cos(t), 0);
				}
			}
		}
	}
	//edge
	for(unsigned i = 0; i <= ny; i++)
	{
		for(unsigned j = 0; j < nx; j++)
		{
			const unsigned p = 42 * nu + 10 * (nx * i + j);
			const unsigned c[] = {
				42 * nu + 10 * ne + 2 * ((nx + 1) * (i + 0) + j + 0) + 0,
				42 * nu + 10 * ne + 2 * ((nx + 1) * (i + 0) + j + 1) + 0,
				42 * nu + 10 * ne + 2 * ((nx + 1) * (i + 0) + j + 0) + 1,
				42 * nu + 10 * ne + 2 * ((nx + 1) * (i + 0) + j + 1) + 1
			};
			//outer
			model->mesh()->add_element(fea::mesh::elements::type::beam3, {p + 0, p + 3});
			model->mesh()->add_element(fea::mesh::elements::type::beam3, {p + 0, p + 9});
			model->mesh()->add_element(fea::mesh::elements::type::beam3, {p + 1, p + 5});
			model->mesh()->add_element(fea::mesh::elements::type::beam3, {p + 1, p + 7});
			//hinge
			if(r != 0)
			{
				model->mesh()->add_element(fea::mesh::elements::type::beam3, {p + 2, c[0]});
				model->mesh()->add_element(fea::mesh::elements::type::beam3, {p + 4, c[1]});
				model->mesh()->add_element(fea::mesh::elements::type::beam3, {p + 6, c[2]});
				model->mesh()->add_element(fea::mesh::elements::type::beam3, {p + 8, c[3]});
			}
			//orientation
			for(unsigned q = 0; q < nd; q++)
			{
				const unsigned p = 4 * nd * nu + nd * (nx * i + j) + q;
				((fea::mesh::elements::Beam3*) model->mesh()->element(p))->orientation(0, 1, 0);
			}
		}
	}
	for(unsigned j = 0; j <= nx; j++)
	{
		for(unsigned i = 0; i < ny; i++)
		{
			const unsigned p = 42 * nu + 10 * nx * (ny + 1) + 10 * (ny * j + i);
			const unsigned c[] = {
				42 * nu + 10 * ne + 2 * ((nx + 1) * (i + 0) + j + 0) + 0,
				42 * nu + 10 * ne + 2 * ((nx + 1) * (i + 1) + j + 0) + 0,
				42 * nu + 10 * ne + 2 * ((nx + 1) * (i + 0) + j + 0) + 1,
				42 * nu + 10 * ne + 2 * ((nx + 1) * (i + 1) + j + 0) + 1
			};
			//outer
			model->mesh()->add_element(fea::mesh::elements::type::beam3, {p + 0, p + 3});
			model->mesh()->add_element(fea::mesh::elements::type::beam3, {p + 0, p + 9});
			model->mesh()->add_element(fea::mesh::elements::type::beam3, {p + 1, p + 5});
			model->mesh()->add_element(fea::mesh::elements::type::beam3, {p + 1, p + 7});
			//hinge
			if(r != 0)
			{
				model->mesh()->add_element(fea::mesh::elements::type::beam3, {p + 2, c[0]});
				model->mesh()->add_element(fea::mesh::elements::type::beam3, {p + 4, c[1]});
				model->mesh()->add_element(fea::mesh::elements::type::beam3, {p + 6, c[2]});
				model->mesh()->add_element(fea::mesh::elements::type::beam3, {p + 8, c[3]});
			}
			//orientation
			for(unsigned q = 0; q < nd; q++)
			{
				const unsigned p = 4 * nd * nu + nd * (ny + 1) * nx + nd * (ny * j + i) + q;
				((fea::mesh::elements::Beam3*) model->mesh()->element(p))->orientation(1, 0, 0);
			}
		}
	}
}
static void refine_elements(fea::models::Model* model)
{
	const unsigned nd = r ? 8 : 4;
	for(unsigned i = 0; i < ny; i++)
	{
		for(unsigned j = 0; j < nx; j++)
		{
			for(unsigned k = 0; k < 4; k++)
			{
				for(unsigned q = 0; q < 4; q++)
				{
					fea::mesh::cells::Line::refine(4 * nd * (j + nx * i) + nd * k + q, nr);
				}
			}
		}
	}
	for(unsigned i = 0; i <= ny; i++)
	{
		for(unsigned j = 0; j < nx; j++)
		{
			for(unsigned q = 0; q < 4; q++)
			{
				fea::mesh::cells::Line::refine(nd * (4 * nu + j + nx * i) + q, nr);
			}
		}
	}
	for(unsigned j = 0; j <= nx; j++)
	{
		for(unsigned i = 0; i < ny; i++)
		{
			for(unsigned q = 0; q < 4; q++)
			{
				fea::mesh::cells::Line::refine(nd * (4 * nu + nx * (ny + 1) + i + ny * j) + q, nr);
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
	//cases
	model->boundary()->add_load_case("Deployment");
	model->boundary()->add_self_weight("Gravity", fea::mesh::nodes::dof::translation_z);
	//loads
	model->boundary()->load_case(0)->add_load_node(42 * nu + 10 * ne + 2 * (nx + 1) * 0 + 0, ux, mat::sign(open) * pr);
	model->boundary()->load_case(0)->add_load_node(42 * nu + 10 * ne + 2 * (nx + 1) * 0 + 1, ux, mat::sign(open) * pr);
	model->boundary()->load_case(0)->add_load_node(42 * nu + 10 * ne + 2 * (nx + 1) * 1 + 0, ux, mat::sign(open) * pr);
	model->boundary()->load_case(0)->add_load_node(42 * nu + 10 * ne + 2 * (nx + 1) * 1 + 1, ux, mat::sign(open) * pr);
}
static void create_supports(fea::models::Model* model)
{
	for(unsigned i = 0; i <= nx; i++)
	{
		model->boundary()->add_support(42 * nu + 10 * ne + 2 * i + 0, uy);
		if(sups)
		{
			model->boundary()->add_support(42 * nu + 10 * ne + 2 * i + 1, uy);
		}
	}
	for(unsigned i = 0; i <= ny; i++)
	{
		model->boundary()->add_support(42 * nu + 10 * ne + 2 * nx + 2 * (nx + 1) * i + 0, ux);
		if(sups)
		{
			model->boundary()->add_support(42 * nu + 10 * ne + 2 * nx + 2 * (nx + 1) * i + 1, ux);
		}
	}
	for(unsigned i = 0; i <= ny; i++)
	{
		for(unsigned j = 0; j <= nx; j++)
		{
			model->boundary()->add_support(42 * nu + 10 * ne + 2 * (nx + 1) * i + 2 * j + 0, uz);
		}
	}
}

static void create_solver(fea::models::Model* model)
{
	//solver
	fea::mesh::elements::Mechanic::geometric(true);
	model->analysis()->solver(fea::analysis::solvers::type::static_nonlinear);
	
	//strategies
	fea::analysis::strategies::type types[] = {
		fea::analysis::strategies::type::control_load,
		fea::analysis::strategies::type::minimal_norm,
		fea::analysis::strategies::type::control_state,
		fea::analysis::strategies::type::arc_length_cylindric
	};
	
	//gravity
	if(grav)
	{
		model->analysis()->solver()->watch_dof(42 * nu + 10 * ne, ux);
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model->analysis()->solver())->load_max(1);
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model->analysis()->solver())->load_case(1);
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model->analysis()->solver())->step_max(100);
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model->analysis()->solver())->tolerance(tl);
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model->analysis()->solver())->strategy(types[0]);
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model->analysis()->solver())->load_predictor(5e-2);
		model->analysis()->solve();
	}
	
	//deployment
	if(depl)
	{
		model->analysis()->solver()->watch_dof(42 * nu + 10 * ne, ux);
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model->analysis()->solver())->dead(grav);
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model->analysis()->solver())->dof_max(ul);
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model->analysis()->solver())->dead_case(1);
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model->analysis()->solver())->load_case(0);
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model->analysis()->solver())->step_max(ns);
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model->analysis()->solver())->load_max(pl);
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model->analysis()->solver())->tolerance(tl);
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model->analysis()->solver())->attempt_max(na);
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model->analysis()->solver())->load_adjust(true);
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model->analysis()->solver())->iteration_max(nm);
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model->analysis()->solver())->load_predictor(d0);
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model->analysis()->solver())->strategy(types[st]);
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model->analysis()->solver())->iteration_desired(ni);
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model->analysis()->solver())->load_increment_min(dp);
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model->analysis()->solver())->load_increment_max(dP);
		model->analysis()->solve(grav);
	}
}

//model
void tests::deployable::static_nonlinear::slut_line(void)
{
	//model
	char name[200], path[200];
//	sprintf(name, "%02d", n);
//	sprintf(name, "%03d", unsigned(mp));
//	sprintf(name, "%02d", unsigned(100 * h));
	sprintf(name, "kr_%02d", unsigned(kr / 1e3));
	sprintf(path, "benchmarks/deployable/static/nonlinear/slut line/models/%s", alte ? "alternate" : "uniform");
	fea::models::Model model(name, path);
	
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
	create_loads(&model);
	create_supports(&model);

	//analysis
	create_solver(&model);
	
	//save
	model.save();
}
