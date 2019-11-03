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
#include "Mesh/Materials/Types.h"
#include "Mesh/Elements/Mechanic/Mechanic.h"

#include "Boundary/Boundary.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Types.h"
#include "Analysis/Solvers/State.h"

//ben
#include "benchmarks/mechanic/deployable.h"

//input
const static double a = 4;
const static double H = 20;
const static double S = 40;
const static unsigned n = 10;

//parameters
const static double f = S / H / 2;
const static double R = H / 2 * (1 + f * f);
const static double q = 2 * asin(2 * f / (1 + f * f));

const static double dq = q / n / 2;
const static double q0 = (M_PI - q) / 2;
const static double dw = asin(R / a * sin(dq)) - dq;

const static double b = sin(dq) / sin(dw - dq) * R;

//angles
static double dt(double t)
{
	return t / n / 2;
}
static double wn(double t) 
{
	return asin(R / a * sin(dt(t))) - dt(t);
}
static double dg(double t)
{
	return atan((b - a) / (b + a) * tan(wn(t)));
}

//base
static double tb(double t, unsigned i)
{
	return q0 + (2 * i + 1) * dg(t);
}
static double xb(double t, unsigned i)
{
	double x0 = 0;
	for(unsigned j = 0; j < i; j++)
	{
		x0 -= 2 * a * sin(wn(t)) * sin(tb(t, j));
	}
	return x0;
};
static double yb(double t, unsigned i)
{
	double y0 = 0;
	for(unsigned j = 0; j < i; j++)
	{
		y0 += 2 * a * sin(wn(t)) * cos(tb(t, j));
	}
	return y0;
};

//positions
static double xl(double t, unsigned i)
{
	switch(i)
	{
		case 0: return 0;
		case 1: return 0;
		case 2: return a * cos(wn(t));
		case 3: return (a + b) * cos(wn(t));
		case 4: return (a + b) * cos(wn(t));
	}
}
static double yl(double t, unsigned i)
{
	switch(i)
	{
		case 0: return 0;
		case 2: return a * sin(wn(t));
		case 1: return 2 * a * sin(wn(t));
		case 3: return (a + b) * sin(wn(t));
		case 4: return (a - b) * sin(wn(t));
	}
}

//motion
static double px(double t, unsigned i, unsigned j)
{
	return xb(t, i) + xl(t, j) * cos(tb(t, i)) - yl(t, j) * sin(tb(t, i));
}
static double py(double t, unsigned i, unsigned j)
{
	return yb(t, i) + xl(t, j) * sin(tb(t, i)) + yl(t, j) * cos(tb(t, i));
}
static double* pn(double* q, double x, double y, double t, double a, double b)
{
	q[2] = 0;
	q[0] = x + a * cos(t) - b * sin(t);
	q[1] = y + a * sin(t) + b * cos(t);
	return q;
}

//model
void tests::deployable::state::arch_polar(void)
{
	//model
	fea::models::Model model("arch polar", "benchmarks/deployable/state");
	
	//check
	if(a < R * sin(dq))
	{
		printf("\tError:\n\tGeometric condition not satisfied!\n\tThe minimum a is: %.2f\n", R * sin(dq));
		return;
	}
	
	//nodes
	double p0[3];
	for(unsigned i = 0; i < n; i++)
	{
		model.mesh()->add_node(pn(p0, xb(q, i), yb(q, i), tb(q, i), xl(q, 0), yl(q, 0)));
		model.mesh()->add_node(pn(p0, xb(q, i), yb(q, i), tb(q, i), xl(q, 1), yl(q, 1)));
		model.mesh()->add_node(pn(p0, xb(q, i), yb(q, i), tb(q, i), xl(q, 2), yl(q, 2)));
		model.mesh()->add_node(pn(p0, xb(q, i), yb(q, i), tb(q, i), xl(q, 2), yl(q, 2)));
		model.mesh()->add_node(pn(p0, xb(q, i), yb(q, i), tb(q, i), xl(q, 3), yl(q, 3)));
		model.mesh()->add_node(pn(p0, xb(q, i), yb(q, i), tb(q, i), xl(q, 4), yl(q, 4)));
	}
	
	//cells
	model.mesh()->add_cell(fea::mesh::cells::type::beam);

	//materials
	model.mesh()->add_material(fea::mesh::materials::type::steel);

	//sections
	model.mesh()->add_section(fea::mesh::sections::type::rectangle);
	
	//elements
	fea::mesh::elements::Mechanic::geometric(true);
	for(unsigned i = 0; i < n; i++)
	{
		model.mesh()->add_element(fea::mesh::elements::type::beam2, {6 * i + 0, 6 * i + 2});
		model.mesh()->add_element(fea::mesh::elements::type::beam2, {6 * i + 2, 6 * i + 4});
		model.mesh()->add_element(fea::mesh::elements::type::beam2, {6 * i + 1, 6 * i + 3});
		model.mesh()->add_element(fea::mesh::elements::type::beam2, {6 * i + 3, 6 * i + 5});
	}
	
	//joints
	for(unsigned i = 0; i < n; i++)
	{
		model.mesh()->add_joint(fea::mesh::joints::type::pinned, {6 * i + 2, 6 * i + 3});
	}
	for(unsigned i = 0; i + 1 < n; i++)
	{
		model.mesh()->add_joint(fea::mesh::joints::type::pinned, {6 * i + 1, 6 * (i + 1) + 0});
		model.mesh()->add_joint(fea::mesh::joints::type::pinned, {6 * i + 4, 6 * (i + 1) + 5});
	}
	for(unsigned i = 0; i < 3 * n - 2; i++)
	{
		((fea::mesh::joints::Pinned*) model.mesh()->joint(i))->fixed(false);
	}
	
	//supports
	for(unsigned i = 0; i < n; i++)
	{
		//states
		const std::function<double(double)> u0 = [i] (double t) { return px(t, i, 0) - px(q, i, 0); };
		const std::function<double(double)> v0 = [i] (double t) { return py(t, i, 0) - py(q, i, 0); };
		const std::function<double(double)> u1 = [i] (double t) { return px(t, i, 1) - px(q, i, 1); };
		const std::function<double(double)> v1 = [i] (double t) { return py(t, i, 1) - py(q, i, 1); };
		const std::function<double(double)> u2 = [i] (double t) { return px(t, i, 2) - px(q, i, 2); };
		const std::function<double(double)> v2 = [i] (double t) { return py(t, i, 2) - py(q, i, 2); };
		const std::function<double(double)> u3 = [i] (double t) { return px(t, i, 3) - px(q, i, 3); };
		const std::function<double(double)> v3 = [i] (double t) { return py(t, i, 3) - py(q, i, 3); };
		const std::function<double(double)> u4 = [i] (double t) { return px(t, i, 4) - px(q, i, 4); };
		const std::function<double(double)> v4 = [i] (double t) { return py(t, i, 4) - py(q, i, 4); };
		const std::function<double(double)> t1 = [i] (double t) { return tb(t, i) + wn(t) - tb(q, i) - wn(q); };
		const std::function<double(double)> t2 = [i] (double t) { return tb(t, i) - wn(t) - tb(q, i) + wn(q); };
		//supports
		model.boundary()->add_support(6 * i + 0, fea::mesh::nodes::dof::rotation_z, t1);
		model.boundary()->add_support(6 * i + 2, fea::mesh::nodes::dof::rotation_z, t1);
		model.boundary()->add_support(6 * i + 4, fea::mesh::nodes::dof::rotation_z, t1);
		model.boundary()->add_support(6 * i + 1, fea::mesh::nodes::dof::rotation_z, t2);
		model.boundary()->add_support(6 * i + 3, fea::mesh::nodes::dof::rotation_z, t2);
		model.boundary()->add_support(6 * i + 5, fea::mesh::nodes::dof::rotation_z, t2);
		model.boundary()->add_support(6 * i + 0, fea::mesh::nodes::dof::translation_x, u0);
		model.boundary()->add_support(6 * i + 0, fea::mesh::nodes::dof::translation_y, v0);
		model.boundary()->add_support(6 * i + 1, fea::mesh::nodes::dof::translation_x, u1);
		model.boundary()->add_support(6 * i + 1, fea::mesh::nodes::dof::translation_y, v1);
		model.boundary()->add_support(6 * i + 2, fea::mesh::nodes::dof::translation_x, u2);
		model.boundary()->add_support(6 * i + 2, fea::mesh::nodes::dof::translation_y, v2);
		model.boundary()->add_support(6 * i + 3, fea::mesh::nodes::dof::translation_x, u2);
		model.boundary()->add_support(6 * i + 3, fea::mesh::nodes::dof::translation_y, v2);
		model.boundary()->add_support(6 * i + 4, fea::mesh::nodes::dof::translation_x, u3);
		model.boundary()->add_support(6 * i + 4, fea::mesh::nodes::dof::translation_y, v3);
		model.boundary()->add_support(6 * i + 5, fea::mesh::nodes::dof::translation_x, u4);
		model.boundary()->add_support(6 * i + 5, fea::mesh::nodes::dof::translation_y, v4);
	}
	
	//solver
	model.analysis()->solver(fea::analysis::solvers::type::state);
	dynamic_cast<fea::analysis::solvers::State*> (model.analysis()->solver())->time_min(0);
	dynamic_cast<fea::analysis::solvers::State*> (model.analysis()->solver())->time_max(q);
	dynamic_cast<fea::analysis::solvers::State*> (model.analysis()->solver())->step_max(500);
	dynamic_cast<fea::analysis::solvers::State*> (model.analysis()->solver())->watch_dof(0, fea::mesh::nodes::dof::rotation_z);

	//solve
	model.analysis()->solve();
	
	//save
	model.save();
}
