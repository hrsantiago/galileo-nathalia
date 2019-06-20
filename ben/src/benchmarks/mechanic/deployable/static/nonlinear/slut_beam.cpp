//std
#include <cmath>

//mat
#include "misc/defs.h"
#include "misc/util.h"
#include "linear/dense.h"

//fea
#include "Models/Model.h"

#include "Mesh/Mesh.h"
#include "Mesh/Nodes/Dofs.h"
#include "Mesh/Nodes/Node.h"
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
const static bool fric = false;

//geometry
const static double L = 1.00e+00;
const static double h = 0.50e+00;
const static double H = 0.40e+00;

//joint
const static double mp = 5.00e+00;
const static double kt = 1.00e+03;
const static double kp = 4.00e+00;
const static double kr = 1.00e+05;

//material
const static double p = 2.70e+03;
const static double E = 7.00e+10;
	
//cross section
const static double hs = 3.00e-02;
const static double ws = 1.00e-02;
const static double ts = 1.00e-03;

//analysis
const static double Pr = 1.00e+00;
const static double dp = 5.00e-01;
const static double dP = 5.00e+03;
const static double pl = 8.00e+03;
const static double tl = 1.00e-04;

const static unsigned st = 1;
const static unsigned nd = 4;
const static unsigned nr = 4;
const static unsigned ni = 3;
const static unsigned nm = 10;
const static unsigned ns = 300;

//parameters
const static double l = sqrt(h * h + L * L);
const static double D = 2 * L * sin(M_PI / nd);
const static double e = sqrt(H * H + D * D) / 2;
const static double x = D * D / (4 * e * l - 2 * h * H);

const static double d = x * l;
const static double c = 2 * e - d;
const static double b = (1 - x) * l;
const static double a = (1 - x) / x * c;
const static double z = h - (1 - x) / x * H;

const static double ul = (2 * e - H) * (1 - x) / x;

//data
static double p0[3], p1[3];
static double po[nd][3], pd[nd][3], pe[nd][3], pc[nd][3];

//create
static void set_points(void)
{
	double t = 0;
	p0[0] = p0[1] = 0;
	p1[0] = p1[1] = 0;
	p0[2] = open ? z : d - a;
	p1[2] = open ? h : d + b;
	for(unsigned i = 0; i < nd; i++)
	{
		t = (2 * i + 1) * M_PI / nd;
		pe[i][2] = 0;
		pd[i][2] = open ? H : d + c;
		po[i][2] = open ? x * h : d;
		pc[i][2] = open ? H / 2 : (d + c) / 2;
		po[i][0] = open ? (1 - x) * L * cos(t) : 0;
		po[i][1] = open ? (1 - x) * L * sin(t) : 0;
		pd[i][0] = pe[i][0] = open ? L * cos(t) : 0;
		pd[i][1] = pe[i][1] = open ? L * sin(t) : 0;
		pc[i][0] = open ? L * cos(M_PI / nd) * cos(t + M_PI / nd) : 0;
		pc[i][1] = open ? L * cos(M_PI / nd) * sin(t + M_PI / nd) : 0;
	}
}
static void check_points(void)
{
	//mass
	printf("Mass:\t%+.2e\n", p * nd * (a + b + c + d));

	//check
	for(unsigned i = 0; i < nd; i++)
	{
		printf("check %d: ", i);
		const unsigned j = i + 1 != nd ? i + 1 : 0;
		printf("%d ", fabs(mat::norm(p0, po[i], 3) - a) < 1e-5 * L);
		printf("%d ", fabs(mat::norm(p1, po[i], 3) - b) < 1e-5 * L);
		printf("%d ", fabs(mat::norm(pd[i], po[i], 3) - c) < 1e-5 * L);
		printf("%d ", fabs(mat::norm(pe[i], po[i], 3) - d) < 1e-5 * L);
		printf("%d ", fabs(mat::norm(pd[i], pc[i], 3) - e) < 1e-5 * L);
		printf("%d ", fabs(mat::norm(pe[i], pc[i], 3) - e) < 1e-5 * L);
		printf("%d ", fabs(mat::norm(pd[j], pc[i], 3) - e) < 1e-5 * L);
		printf("%d ", fabs(mat::norm(pe[j], pc[i], 3) - e) < 1e-5 * L);
		printf("\n");
	}
}

static void create_nodes(fea::models::Model* model)
{
	model->mesh()->add_node(p0);
	model->mesh()->add_node(p1);
	for(unsigned i = 0; i < nd; i++)
	{
		model->mesh()->add_node(p0);
		model->mesh()->add_node(p1);
		model->mesh()->add_node(po[i]);
		model->mesh()->add_node(po[i]);
		model->mesh()->add_node(pd[i]);
		model->mesh()->add_node(pd[i]);
		model->mesh()->add_node(pd[i]);
		model->mesh()->add_node(pd[i]);
		model->mesh()->add_node(pe[i]);
		model->mesh()->add_node(pe[i]);
		model->mesh()->add_node(pe[i]);
		model->mesh()->add_node(pe[i]);
		model->mesh()->add_node(pc[i]);
		model->mesh()->add_node(pc[i]);
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
		const double c0 = cos((2 * i + 0) * M_PI / nd);
		const double s0 = sin((2 * i + 0) * M_PI / nd);
		const double c1 = cos((2 * i + 1) * M_PI / nd);
		const double s1 = sin((2 * i + 1) * M_PI / nd);
		const double c2 = cos((2 * i + 2) * M_PI / nd);
		const double s2 = sin((2 * i + 2) * M_PI / nd);
		model->mesh()->add_joint(fea::mesh::joints::type::hinge, {0, 14 * i +  2});
		model->mesh()->add_joint(fea::mesh::joints::type::hinge, {1, 14 * i +  3});
		model->mesh()->add_joint(fea::mesh::joints::type::hinge, {14 * i +  4, 14 * i +  5});
		model->mesh()->add_joint(fea::mesh::joints::type::hinge, {14 * i +  6, 14 * i +  7});
		model->mesh()->add_joint(fea::mesh::joints::type::hinge, {14 * i +  6, 14 * i +  8});
		model->mesh()->add_joint(fea::mesh::joints::type::hinge, {14 * i +  6, 14 * i +  9});
		model->mesh()->add_joint(fea::mesh::joints::type::hinge, {14 * i + 10, 14 * i + 11});
		model->mesh()->add_joint(fea::mesh::joints::type::hinge, {14 * i + 10, 14 * i + 12});
		model->mesh()->add_joint(fea::mesh::joints::type::hinge, {14 * i + 10, 14 * i + 13});
		model->mesh()->add_joint(fea::mesh::joints::type::hinge, {14 * i + 14, 14 * i + 15});
		((fea::mesh::joints::Hinge*) model->mesh()->joint(10 * i + 0))->axis(+s1, -c1, 0);
		((fea::mesh::joints::Hinge*) model->mesh()->joint(10 * i + 1))->axis(+s1, -c1, 0);
		((fea::mesh::joints::Hinge*) model->mesh()->joint(10 * i + 2))->axis(+s1, -c1, 0);
		((fea::mesh::joints::Hinge*) model->mesh()->joint(10 * i + 3))->axis(+s1, -c1, 0);
		((fea::mesh::joints::Hinge*) model->mesh()->joint(10 * i + 4))->axis(+c2, +s2, 0);
		((fea::mesh::joints::Hinge*) model->mesh()->joint(10 * i + 5))->axis(+c0, +s0, 0);
		((fea::mesh::joints::Hinge*) model->mesh()->joint(10 * i + 6))->axis(+s1, -c1, 0);
		((fea::mesh::joints::Hinge*) model->mesh()->joint(10 * i + 7))->axis(+c2, +s2, 0);
		((fea::mesh::joints::Hinge*) model->mesh()->joint(10 * i + 8))->axis(+c0, +s0, 0);
		((fea::mesh::joints::Hinge*) model->mesh()->joint(10 * i + 9))->axis(+c2, +s2, 0);
		((fea::mesh::joints::Hinge*) model->mesh()->joint(10 * i + 0))->orientation(+c1, +s1, 0);
		((fea::mesh::joints::Hinge*) model->mesh()->joint(10 * i + 1))->orientation(+c1, +s1, 0);
		((fea::mesh::joints::Hinge*) model->mesh()->joint(10 * i + 2))->orientation(+c1, +s1, 0);
		((fea::mesh::joints::Hinge*) model->mesh()->joint(10 * i + 3))->orientation(+c1, +s1, 0);
		((fea::mesh::joints::Hinge*) model->mesh()->joint(10 * i + 4))->orientation(+s2, -c2, 0);
		((fea::mesh::joints::Hinge*) model->mesh()->joint(10 * i + 5))->orientation(+s0, -c0, 0);
		((fea::mesh::joints::Hinge*) model->mesh()->joint(10 * i + 6))->orientation(+c1, +s1, 0);
		((fea::mesh::joints::Hinge*) model->mesh()->joint(10 * i + 7))->orientation(+s2, -c2, 0);
		((fea::mesh::joints::Hinge*) model->mesh()->joint(10 * i + 8))->orientation(+s0, -c0, 0);
		((fea::mesh::joints::Hinge*) model->mesh()->joint(10 * i + 9))->orientation(+s2, -c2, 0);
		for(unsigned j = 0; j < 10; j++)
		{
			if(fric)
			{
				((fea::mesh::joints::Hinge*) model->mesh()->joint(10 * i + j))->moment_yield(mp);
				((fea::mesh::joints::Hinge*) model->mesh()->joint(10 * i + j))->plastic_modulus(kp);
				((fea::mesh::joints::Hinge*) model->mesh()->joint(10 * i + j))->stiffness(0, 2, kt);
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
	for(unsigned i = 0; i < nd; i++)
	{
		const double t = (2 * i + 1) * M_PI / nd;
		const unsigned j = i + 1 != nd ? i + 1 : 0;
		model->mesh()->add_element(fea::mesh::elements::type::beam3, {14 * i +  4, 14 * i +  2});
		model->mesh()->add_element(fea::mesh::elements::type::beam3, {14 * i +  4, 14 * i +  7});
		model->mesh()->add_element(fea::mesh::elements::type::beam3, {14 * i +  5, 14 * i +  3});
		model->mesh()->add_element(fea::mesh::elements::type::beam3, {14 * i +  5, 14 * i + 11});
		model->mesh()->add_element(fea::mesh::elements::type::beam3, {14 * i + 14, 14 * i +  8});
		model->mesh()->add_element(fea::mesh::elements::type::beam3, {14 * i + 15, 14 * i + 12});
		model->mesh()->add_element(fea::mesh::elements::type::beam3, {14 * i + 14, 14 * j + 13});
		model->mesh()->add_element(fea::mesh::elements::type::beam3, {14 * i + 15, 14 * j +  9});
		((fea::mesh::elements::Beam3*) model->mesh()->element(8 * i + 0))->orientation(-sin(t), cos(t), 0);
		((fea::mesh::elements::Beam3*) model->mesh()->element(8 * i + 1))->orientation(-sin(t), cos(t), 0);
		((fea::mesh::elements::Beam3*) model->mesh()->element(8 * i + 2))->orientation(-sin(t), cos(t), 0);
		((fea::mesh::elements::Beam3*) model->mesh()->element(8 * i + 3))->orientation(-sin(t), cos(t), 0);
		((fea::mesh::elements::Beam3*) model->mesh()->element(8 * i + 4))->orientation(cos(t + M_PI / nd), sin(t + M_PI / nd), 0);
		((fea::mesh::elements::Beam3*) model->mesh()->element(8 * i + 5))->orientation(cos(t + M_PI / nd), sin(t + M_PI / nd), 0);
		((fea::mesh::elements::Beam3*) model->mesh()->element(8 * i + 6))->orientation(cos(t + M_PI / nd), sin(t + M_PI / nd), 0);
		((fea::mesh::elements::Beam3*) model->mesh()->element(8 * i + 7))->orientation(cos(t + M_PI / nd), sin(t + M_PI / nd), 0);
	}
}
static void refine_elements(fea::models::Model* model)
{
	for(unsigned i = 0; i < 8 * nd; i++)
	{
		fea::mesh::cells::Line::refine(i, nr);
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
	model->boundary()->add_load_case("Deployment");
	model->boundary()->add_self_weight("Gravity", fea::mesh::nodes::dof::translation_z);
	for(unsigned i = 0; i < nd; i++)
	{
		const double t = (2 * i + 1) * M_PI / nd;
		model->boundary()->load_case(0)->add_load_node(14 * i +  6, fea::mesh::nodes::dof::translation_x, -mat::sign(open) * Pr * cos(t));
		model->boundary()->load_case(0)->add_load_node(14 * i +  6, fea::mesh::nodes::dof::translation_y, -mat::sign(open) * Pr * sin(t));
		model->boundary()->load_case(0)->add_load_node(14 * i + 10, fea::mesh::nodes::dof::translation_x, -mat::sign(open) * Pr * cos(t));
		model->boundary()->load_case(0)->add_load_node(14 * i + 10, fea::mesh::nodes::dof::translation_y, -mat::sign(open) * Pr * sin(t));
	}
}
static void create_initial(fea::models::Model* model)
{
	if(!open)
	{
		for(unsigned i = 0; i < nd; i++)
		{
			const double t = (2 * i + 1) * M_PI / nd;
			model->boundary()->add_initial(14 * i + 4, fea::mesh::nodes::dof::translation_x, 1e-3 * L * cos(t), 0);
			model->boundary()->add_initial(14 * i + 5, fea::mesh::nodes::dof::translation_y, 1e-3 * L * sin(t), 0);
		}
	}
}
static void create_supports(fea::models::Model* model)
{
	model->boundary()->add_support(0, fea::mesh::nodes::dof::rotation_x);
	model->boundary()->add_support(0, fea::mesh::nodes::dof::rotation_y);
	model->boundary()->add_support(0, fea::mesh::nodes::dof::rotation_z);
	model->boundary()->add_support(1, fea::mesh::nodes::dof::rotation_x);
	model->boundary()->add_support(1, fea::mesh::nodes::dof::rotation_y);
	model->boundary()->add_support(1, fea::mesh::nodes::dof::rotation_z);
	model->boundary()->add_support(0, fea::mesh::nodes::dof::translation_x);
	model->boundary()->add_support(0, fea::mesh::nodes::dof::translation_y);
	model->boundary()->add_support(0, fea::mesh::nodes::dof::translation_z);
	model->boundary()->add_support(1, fea::mesh::nodes::dof::translation_x);
	model->boundary()->add_support(1, fea::mesh::nodes::dof::translation_y);
	for(unsigned i = 0; i < nd; i++)
	{
		model->boundary()->add_support(14 * i +  6, fea::mesh::nodes::dof::rotation_z);
		model->boundary()->add_support(14 * i + 10, fea::mesh::nodes::dof::rotation_z);
	}
}

static void create_solver(fea::models::Model* model)
{
	fea::mesh::elements::Mechanic::geometric(true);
	model->analysis()->solver(fea::analysis::solvers::type::static_nonlinear);
	model->analysis()->solver()->watch_dof(1, fea::mesh::nodes::dof::translation_z);

	//solve gravity
	if(grav)
	{
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model->analysis()->solver())->load_case(1);
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model->analysis()->solver())->step_max(100);
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model->analysis()->solver())->load_max(1e0);
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model->analysis()->solver())->load_predictor(5e-2);
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model->analysis()->solver())->strategy(fea::analysis::strategies::type::control_load);
		model->analysis()->solve();
	}
	
	//gravity displacement
	const double ug = model->mesh()->node(1)->state(fea::mesh::nodes::dof::translation_z);
	
	//strategies
	fea::analysis::strategies::type types[] = {
		fea::analysis::strategies::type::minimal_norm,
		fea::analysis::strategies::type::control_state,
		fea::analysis::strategies::type::arc_length_cylindric
	};
	
	//solve deployment
	if(depl)
	{
		const double ui = open ? ug : -0.999 * ul;
		const double uj = open ? +0.999 * ul : ug;
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model->analysis()->solver())->dead(grav);
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model->analysis()->solver())->dof_min(ui);
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model->analysis()->solver())->dof_max(uj);
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model->analysis()->solver())->dead_case(1);
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model->analysis()->solver())->load_case(0);
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model->analysis()->solver())->step_max(ns);
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model->analysis()->solver())->load_max(pl);
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model->analysis()->solver())->tolerance(tl);
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model->analysis()->solver())->iteration_max(nm);
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model->analysis()->solver())->load_adjust(true);
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model->analysis()->solver())->load_predictor(dp);
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model->analysis()->solver())->strategy(types[st]);
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model->analysis()->solver())->iteration_desired(ni);
		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model->analysis()->solver())->load_increment_max(dP);
//		dynamic_cast<fea::analysis::solvers::Static_Nonlinear*> (model->analysis()->solver())->unload().state = 0.998 * ul;
		model->analysis()->solve(grav);
	}
}

void tests::deployable::static_nonlinear::slut_beam(void)
{
	//model
	fea::models::Model model("slut beam", "benchmarks/deployable/static/nonlinear");

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
	create_initial(&model);
	create_supports(&model);

	//analysis
	create_solver(&model);

	//save
	model.save();
}

