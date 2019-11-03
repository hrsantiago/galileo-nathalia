//std
#include <cmath>

//fea
#include "Model/Model.h"

#include "Mesh/Mesh.h"
#include "Mesh/Nodes/Dofs.h"
#include "Mesh/Cells/Types.h"
#include "Mesh/Sections/Ring.h"
#include "Mesh/Sections/Types.h"
#include "Mesh/Elements/Types.h"
#include "Mesh/Materials/Types.h"
#include "Mesh/Elements/Mechanic/Mechanic.h"
#include "Mesh/Materials/Mechanic/Associative/Steel.h"

#include "Boundary/Boundary.h"
#include "Boundary/Cases/Load_Case.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Types.h"
#include "Analysis/Solvers/Dynamic_Nonlinear.h"

//ben
#include "benchmarks/mechanic/bar.h"

void tests::bar::dynamic_nonlinear::tent_roof(void) 
{
	//model
	fea::models::Model model("tent roof", "benchmarks/bar/dynamic/nonlinear");

	//parameters
	const double g = 9.81e+0;
	const double p = 1.00e+3;
	const double l = 2.00e+1;
	const double h = 2.50e+0;
	const double H = 2.00e+0;
	const double E = 2.00e+9;
	const double r = 7.85e+3;
	const double d = 5.08e-1;
	const double t = 1.27e-2;
	const double w = 1.00e+0;

	const unsigned nx = 10;
	const unsigned ny = 10;
	const unsigned nn = (nx + 1) * (ny + 1);
	const unsigned nw = (nx + 1) * ny / 2 + nx / 2;

	const double q = 4 * h / l;
	const double A = M_PI * (d * d - (d - 2 * t) * (d - 2 * t)) / 4;
	const double s = h == 0 ? l : l / 2 * (sqrt(1 + q * q) + asinh(q) / q);

	//points
	const double ds = s / nx;
	double x[nx + 1], z[nx + 1];
	for(unsigned i = 0; i <= nx; i++)
	{
		if(i == 0)
		{
			z[i] = 0;
			x[i] = -l / 2;
		}
		else
		{
			double r, dr;
			x[i] = x[i - 1] + ds;
			z[i] = h * (1 - pow(2 * x[i] / l, 2));
			while(true)
			{
				r = pow(x[i] - x[i - 1], 2) + pow(z[i] - z[i - 1], 2) - ds * ds;
				if(fabs(r) < 1e-10 * s)
				{
					break;
				}
				dr = 2 * (x[i] - x[i - 1]) - 16 * x[i] * h / (l * l) * (z[i] - z[i - 1]);
				x[i] += -r / dr;
				z[i] = h * (1 - pow(2 * x[i] / l, 2));
			}
		}
	}
	
	//nodes
	for(unsigned i = 0; i <= ny; i++) 
	{
		for(unsigned j = 0; j <= nx; j++)
		{
			model.mesh()->add_node(x[j], i * ds, z[j]);
		}
	}
	for(unsigned i = 0; i < ny; i++) 
	{
		for(unsigned j = 0; j < nx; j++)
		{
			const double dx = x[j + 1] - x[j];
			const double dz = z[j + 1] - z[j];
			const double ym = ds * i + ds / 2;
			const double xm = (x[j + 1] + x[j]) / 2;
			const double zm = (z[j + 1] + z[j]) / 2;
			model.mesh()->add_node(xm - H * dz / ds, ym, zm + H * dx / ds);
		}
	}

	//cells
	model.mesh()->add_cell(fea::mesh::cells::type::bar);

	//materials
	model.mesh()->add_material(fea::mesh::materials::type::steel);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->specific_mass(r);
	((fea::mesh::materials::Steel*) model.mesh()->material(0))->elastic_modulus(E);

	//sections
	model.mesh()->add_section(fea::mesh::sections::type::ring);
	((fea::mesh::sections::Ring*) model.mesh()->section(0))->diameter(d);
	((fea::mesh::sections::Ring*) model.mesh()->section(0))->thickness(t);

	//elements
	fea::mesh::elements::Mechanic::geometric(true);
	fea::mesh::elements::Mechanic::inelastic(true);
	for(unsigned i = 0; i < ny; i++) 
	{
		for(unsigned j = 0; j < nx; j++)
		{
			model.mesh()->add_element(fea::mesh::elements::type::bar, {(nx + 1) * (i + 0) + j + 0, nn + nx * i + j});
			model.mesh()->add_element(fea::mesh::elements::type::bar, {(nx + 1) * (i + 0) + j + 1, nn + nx * i + j});
			model.mesh()->add_element(fea::mesh::elements::type::bar, {(nx + 1) * (i + 1) + j + 1, nn + nx * i + j});
			model.mesh()->add_element(fea::mesh::elements::type::bar, {(nx + 1) * (i + 1) + j + 0, nn + nx * i + j});
			model.mesh()->add_element(fea::mesh::elements::type::bar, {(nx + 1) * (i + 0) + j + 0, (nx + 1) * (i + 0) + j + 1});
			model.mesh()->add_element(fea::mesh::elements::type::bar, {(nx + 1) * (i + 0) + j + 0, (nx + 1) * (i + 1) + j + 0});
			if(j + 1 < nx)
			{
				model.mesh()->add_element(fea::mesh::elements::type::bar, {nn + nx * (i + 0) + j + 0, nn + nx * (i + 0) + j + 1});
			}
			else
			{
				model.mesh()->add_element(fea::mesh::elements::type::bar, {(nx + 1) * (i + 0) + j + 1, (nx + 1) * (i + 1) + j + 1});
			}
			if(i + 1 < ny)
			{
				model.mesh()->add_element(fea::mesh::elements::type::bar, {nn + nx * (i + 0) + j + 0, nn + nx * (i + 1) + j + 0});
			}
			else
			{
				model.mesh()->add_element(fea::mesh::elements::type::bar, {(nx + 1) * (i + 1) + j + 0, (nx + 1) * (i + 1) + j + 1});
			}
		}
	}
	//supports
	for(unsigned i = 0; i <= nx; i++)
	{
		model.boundary()->add_support(i, fea::mesh::nodes::dof::translation_x);
		model.boundary()->add_support(i, fea::mesh::nodes::dof::translation_y);
		model.boundary()->add_support(i, fea::mesh::nodes::dof::translation_z);
		model.boundary()->add_support(ny * (nx + 1) + i, fea::mesh::nodes::dof::translation_x);
		model.boundary()->add_support(ny * (nx + 1) + i, fea::mesh::nodes::dof::translation_y);
		model.boundary()->add_support(ny * (nx + 1) + i, fea::mesh::nodes::dof::translation_z);
	}
	for(unsigned i = 1; i < ny; i++)
	{
		model.boundary()->add_support((nx + 1) * i, fea::mesh::nodes::dof::translation_x);
		model.boundary()->add_support((nx + 1) * i, fea::mesh::nodes::dof::translation_y);
		model.boundary()->add_support((nx + 1) * i, fea::mesh::nodes::dof::translation_z);
		model.boundary()->add_support((nx + 1) * i + nx, fea::mesh::nodes::dof::translation_x);
		model.boundary()->add_support((nx + 1) * i + nx, fea::mesh::nodes::dof::translation_y);
		model.boundary()->add_support((nx + 1) * i + nx, fea::mesh::nodes::dof::translation_z);
	}

	//overload
	model.boundary()->add_load_case();
	for(unsigned i = 0; i + 1 < ny; i++)
	{
		for(unsigned j = 0; j + 1 < nx; j++)
		{
			std::function<double(double)> f = [w] (double t) { return 50 * sin(w * t); };
			model.boundary()->load_case(0)->add_load_node(nn + nx * (i + 0) + j + 0, fea::mesh::nodes::dof::translation_z, -p * ds * ds / 4, f);
			model.boundary()->load_case(0)->add_load_node(nn + nx * (i + 0) + j + 1, fea::mesh::nodes::dof::translation_z, -p * ds * ds / 4, f);
			model.boundary()->load_case(0)->add_load_node(nn + nx * (i + 1) + j + 1, fea::mesh::nodes::dof::translation_z, -p * ds * ds / 4, f);
			model.boundary()->load_case(0)->add_load_node(nn + nx * (i + 1) + j + 0, fea::mesh::nodes::dof::translation_z, -p * ds * ds / 4, f);
		}
	}

	//solve
	model.analysis()->solver(fea::analysis::solvers::type::dynamic_nonlinear);
	dynamic_cast<fea::analysis::solvers::Dynamic_Nonlinear*> (model.analysis()->solver())->time_max(30);
	dynamic_cast<fea::analysis::solvers::Dynamic_Nonlinear*> (model.analysis()->solver())->step_max(8000);
	dynamic_cast<fea::analysis::solvers::Dynamic_Nonlinear*> (model.analysis()->solver())->watch_dof(nw, fea::mesh::nodes::dof::translation_z);
	dynamic_cast<fea::analysis::solvers::Dynamic_Nonlinear*> (model.analysis()->solver())->integration(fea::analysis::solvers::integration::newmark);
	
	//solve
	model.analysis()->solve();
	
	//save
	model.save();
}
