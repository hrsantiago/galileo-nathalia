//std
#include <cmath>

//mat
#include "misc/defs.h"

//fea
#include "Model/Model.h"

#include "Mesh/Mesh.h"
#include "Mesh/Nodes/Dofs.h"
#include "Mesh/Cells/Types.h"
#include "Mesh/Joints/Types.h"
#include "Mesh/Sections/Types.h"
#include "Mesh/Elements/Types.h"
#include "Mesh/Materials/Types.h"
#include "Mesh/Elements/Mechanic/Mechanic.h"

#include "Boundary/Boundary.h"
#include "Boundary/Supports/Support.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Types.h"
#include "Analysis/Solvers/State.h"

//ben
#include "benchmarks/mechanic/deployable.h"

void tests::deployable::state::slut_roof(void)
{
	/*
	Deployable roof
	Literature: International Journal of Steel Structures - J. Cai et al. (2016) pp. 755-764
	 */
	 
	//model
	fea::models::Model model("slut roof", "benchmarks/deployable/state");

	//parameters
	const double l = 1;
	const unsigned n = 12;
	
	const double a = 2 * M_PI / n;
	
	//nodes
	const double po = 0;
	const double pa = l;
	const double pd = 2 * l * cos(a / 2);
	const double pb = l * (1 + 2 * cos(a));
	const double pe = 2 * l * (cos(a / 2) + cos(a));
	const double pc = l * (1 + 2 * cos(a) + 2 * cos(2 * a));
	for(unsigned i = 0; i < n; i++)
	{
		const double ca = cos(i * a);
		const double sa = sin(i * a);
		const double ct = cos(i * a + a / 2);
		const double st = sin(i * a + a / 2);
		model.mesh()->add_node(po * ca, po * sa, 0);
		model.mesh()->add_node(pa * ca, pa * sa, 0);
		model.mesh()->add_node(pb * ca, pb * sa, 0);
		model.mesh()->add_node(pc * ca, pc * sa, 0);
		model.mesh()->add_node(pd * ct, pd * st, 0);
		model.mesh()->add_node(pe * ct, pe * st, 0);
	}
	
	//cells
	model.mesh()->add_cell(fea::mesh::cells::type::bar);

	//materials
	model.mesh()->add_material(fea::mesh::materials::type::steel);

	//sections
	model.mesh()->add_section(fea::mesh::sections::type::rectangle);

	//elements
	fea::mesh::elements::Mechanic::geometric(true);
	for(unsigned i = 0; i < n; i++)
	{
		model.mesh()->add_element(fea::mesh::elements::type::bar, {6 * i + 0, 6 * i + 1});
		model.mesh()->add_element(fea::mesh::elements::type::bar, {6 * i + 4, 6 * i + 1});
		model.mesh()->add_element(fea::mesh::elements::type::bar, {6 * i + 4, 6 * i + 2});
		model.mesh()->add_element(fea::mesh::elements::type::bar, {6 * i + 5, 6 * i + 2});
		model.mesh()->add_element(fea::mesh::elements::type::bar, {6 * i + 5, 6 * i + 3});
		model.mesh()->add_element(fea::mesh::elements::type::bar, {6 * i + 0, i + 1 != n ? 6 * (i + 1) + 1 : 1});
		model.mesh()->add_element(fea::mesh::elements::type::bar, {6 * i + 4, i + 1 != n ? 6 * (i + 1) + 1 : 1});
		model.mesh()->add_element(fea::mesh::elements::type::bar, {6 * i + 4, i + 1 != n ? 6 * (i + 1) + 2 : 2});
		model.mesh()->add_element(fea::mesh::elements::type::bar, {6 * i + 5, i + 1 != n ? 6 * (i + 1) + 2 : 2});
		model.mesh()->add_element(fea::mesh::elements::type::bar, {6 * i + 5, i + 1 != n ? 6 * (i + 1) + 3 : 3});
	}
	
	//supports
	for(unsigned i = 0; i < n; i++)
	{
		const double ct = cos(i * a);
		const double st = sin(i * a);
		const double ca = cos(i * a + a / 2);
		const double sa = sin(i * a + a / 2);
		for(unsigned j = 0; j < 6; j++)
		{
			model.boundary()->add_support(6 * i + j, fea::mesh::nodes::dof::translation_x);
			model.boundary()->add_support(6 * i + j, fea::mesh::nodes::dof::translation_y);
		}
		std::function<double(double)> xo = [l, a] (double t) { return l * sin(t / 2) / sin(a / 2); };
		std::function<double(double)> xa = [l, a] (double t) { return l * sin((a + t) / 2) / sin(a / 2); };
		std::function<double(double)> xb = [l, a] (double t) { return l * sin((a + t) / 2) / sin(a / 2) + 2 * l * cos(a + t / 2); };
		std::function<double(double)> xc = [l, a] (double t) { return l * sin((5 * a + t) / 2) / sin(a / 2); };
		std::function<double(double)> xd = [l, a] (double t) { return l * sin(t / 2) / sin(a / 2) + l * sqrt(2 * (1 + cos(a + t))); };
		std::function<double(double)> xe = [l, a] (double t) { return l * sin(t / 2) / sin(a / 2) + l * sqrt(2 * (1 + cos(a + t))) + l * sqrt(2 * (1 + cos(3 * a + t))); };
		model.boundary()->support(12 * i + 2 * 0 + 0)->state([ca, po, xo] (double t) { return (xo(t) - po) * ca; });
		model.boundary()->support(12 * i + 2 * 0 + 1)->state([sa, po, xo] (double t) { return (xo(t) - po) * sa; });
		model.boundary()->support(12 * i + 2 * 1 + 0)->state([ct, pa, xa] (double t) { return (xa(t) - pa) * ct; });
		model.boundary()->support(12 * i + 2 * 1 + 1)->state([st, pa, xa] (double t) { return (xa(t) - pa) * st; });
		model.boundary()->support(12 * i + 2 * 2 + 0)->state([ct, pb, xb] (double t) { return (xb(t) - pb) * ct; });
		model.boundary()->support(12 * i + 2 * 2 + 1)->state([st, pb, xb] (double t) { return (xb(t) - pb) * st; });
		model.boundary()->support(12 * i + 2 * 3 + 0)->state([ct, pc, xc] (double t) { return (xc(t) - pc) * ct; });
		model.boundary()->support(12 * i + 2 * 3 + 1)->state([st, pc, xc] (double t) { return (xc(t) - pc) * st; });
		model.boundary()->support(12 * i + 2 * 4 + 0)->state([ca, pd, xd] (double t) { return (xd(t) - pd) * ca; });
		model.boundary()->support(12 * i + 2 * 4 + 1)->state([sa, pd, xd] (double t) { return (xd(t) - pd) * sa; });
		model.boundary()->support(12 * i + 2 * 5 + 0)->state([ca, pe, xe] (double t) { return (xe(t) - pe) * ca; });
		model.boundary()->support(12 * i + 2 * 5 + 1)->state([sa, pe, xe] (double t) { return (xe(t) - pe) * sa; });
	}

	//solver
	model.analysis()->solver(fea::analysis::solvers::type::state);
	dynamic_cast<fea::analysis::solvers::State*> (model.analysis()->solver())->time_max(M_PI - 5 * a);
	dynamic_cast<fea::analysis::solvers::State*> (model.analysis()->solver())->watch_dof(0, fea::mesh::nodes::dof::translation_x);

	//solve
	model.analysis()->solve();
	
	//save
	model.save();
}
