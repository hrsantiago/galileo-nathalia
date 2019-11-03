//std
#include <cmath>
#include <cstring>

//mat
#include "misc/stress.h"
#include "linear/dense.h"

//fea
#include "Mesh/Nodes/Dofs.h"
#include "Mesh/Nodes/Node.h"

#include "Mesh/Cells/Types.h"
#include "Mesh/Cells/Line/Line.h"

#include "Mesh/Materials/Mechanic/Mechanic.h"

#include "Mesh/Sections/Section.h"

#include "Mesh/Elements/Types.h"
#include "Mesh/Elements/States.h"
#include "Mesh/Elements/Mechanic/Frame/Bar.h"

#include "Mesh/Points/Mechanic/Fiber.h"
#include "Mesh/Points/Mechanic/Section.h"
#include "Mesh/Points/Mechanic/Mechanic.h"

#include "Boundary/Loads/Types.h"
#include "Boundary/Loads/Elements/Element.h"
#include "Boundary/Loads/Elements/Mechanic/Frame/Axial.h"

namespace fea
{
	namespace mesh
	{
		namespace elements
		{
			//constructors
			Bar::Bar(void) : m_cable(false), m_l(0), m_f(0), m_k(0), m_sr(0)
			{
				return;
			}

			//destructor
			Bar::~Bar(void)
			{
				return;
			}
			
			//serialization
			void Bar::load(FILE* file)
			{
				Frame::load(file);
				fscanf(file, "%d %lf %lf", &m_cable, &m_l, &m_sr);
			}
			void Bar::save(FILE* file) const
			{
				Frame::save(file);
				fprintf(file, "%01d %+.6e %+.6e ", m_cable, m_l, m_sr);
			}

			//types
			unsigned Bar::cells(void) const
			{
				return (unsigned) cells::type::bar;
			}
			unsigned Bar::loads(void) const
			{
				return (unsigned) boundary::loads::type::axial;
			}
			unsigned Bar::states(void) const
			{
				return (unsigned) elements::state::axial;
			}
			unsigned Bar::stresses(void) const
			{
				return (unsigned) mat::stress::sxx;
			}
			unsigned Bar::dofs(unsigned) const
			{
				return 
					(unsigned) nodes::dof::translation_x |
					(unsigned) nodes::dof::translation_y |
					(unsigned) nodes::dof::translation_z;
			}
			elements::type Bar::type(void) const
			{
				return elements::type::bar;
			}

			//data
			bool Bar::cable(void) const
			{
				return m_cable;
			}
			bool Bar::cable(bool cable)
			{
				return m_cable = cable;
			}
			
			bool Bar::strain(void)
			{
				return m_strain;
			}
			bool Bar::strain(bool strain)
			{
				return m_strain = strain;
			}
			
			double Bar::length(void) const
			{
				return m_l;
			}
			double Bar::length(double length)
			{
				return m_l = length;
			}
			
			double Bar::residual_stress(void) const
			{
				return m_sr;
			}
			double Bar::residual_stress(double sr)
			{
				return m_sr = sr;
			}

			//analysis
			void Bar::apply(void)
			{
				//kinematics
				double xi[3], xj[3];
				node(0)->position(xi);
				node(1)->position(xj);
				const double* Xi = node(0)->coordinates();
				const double* Xj = node(1)->coordinates();
				//displacements
				double ur[3];
				double ui[3], uj[3];
				mat::sub(uj, xj, Xj, 3);
				mat::sub(ui, xi, Xi, 3);
				mat::sub(ur, uj, ui, 3);
				//length
				const double l = mat::norm(xj, xi, 3);
				const double L = m_l != 0 ? m_l : mat::norm(Xj, Xi, 3);
				//strech
				const double a = m_geometric ? l / L : 1;
				//direction
				double s1[3];
				mat::normalize(s1, Xj, Xi, 3);
				//section
				const double A = ((cells::Line*) cell())->section()->area();
				//material
				const double E = ((materials::Mechanic*) material(0))->elastic_modulus();
				//material point
				points::Mechanic& mp = ((points::Section*) m_points[0])->m_fibers[0].m_point;
				//strain
				const double el = log(a);
				const double eg = (a * a - 1) / 2;
				const double es = mat::dot(s1, ur, 3) / L;
				const double em = m_geometric ? m_strain ? el : eg : es;
				const double de = m_geometric ? m_strain ? +1 / a : a : 1;
				const double he = m_geometric ? m_strain ? -1 / (a * a) : 1 : 0;
				//return mapping
				double s, C = E;
				if(!m_inelastic)
				{
					s = E * em + m_sr;
				}
				else
				{
					((materials::Mechanic*) material(0))->return_mapping(&s, &C, &em, &m_sr, mp);
				}
				//local force and stiffness
				m_f = m_cable && s < 0 ? 0 : s * A * de;
				m_k = m_cable && s < 0 ? 0 : (de * de * C + he * s) * A / L;
			}
			
			void Bar::record(void)
			{
				char formatter[200];
				sprintf(formatter, "%+.6e %+.6e\n", m_f, m_f);
				m_results += formatter;
			}

			//formulation
			double* Bar::inertial_force(double* f) const
			{
				//coordinates
				const double* Xi = node(0)->coordinates();
				const double* Xj = node(1)->coordinates();
				//acceleration
				const double* ai = node(0)->translation(2);
				const double* aj = node(1)->translation(2);
				//length
				const double L = mat::norm(Xj, Xi, 3);
				//material
				const double p = material()->specific_mass();
				//section
				const double A = ((cells::Line*) cell())->section()->area();
				//inertial force
				f[0] = p * A * L / 6 * (2 * ai[0] + aj[0]);
				f[1] = p * A * L / 6 * (2 * ai[1] + aj[1]);
				f[2] = p * A * L / 6 * (2 * ai[2] + aj[2]);
				f[3] = p * A * L / 6 * (2 * aj[0] + ai[0]);
				f[4] = p * A * L / 6 * (2 * aj[1] + ai[1]);
				f[5] = p * A * L / 6 * (2 * aj[2] + ai[2]);
				return f;
			}
			double* Bar::internal_force(double* f) const
			{
				//kinematics
				double pi[3], pj[3];
				const double* xi = m_geometric ? node(0)->position(pi) : node(0)->coordinates();
				const double* xj = m_geometric ? node(1)->position(pj) : node(1)->coordinates();
				//direction
				double t1[3];
				mat::normalize(t1, xj, xi, 3);
				//internal force
				f[0] = -m_f * t1[0];
				f[1] = -m_f * t1[1];
				f[2] = -m_f * t1[2];
				f[3] = +m_f * t1[0];
				f[4] = +m_f * t1[1];
				f[5] = +m_f * t1[2];
				return f;
			}
			
			double* Bar::reference_force(double* f, const boundary::loads::Element* load) const
			{
				switch(load->type())
				{
					case boundary::loads::type::axial: 
						return load_axial(f, (const boundary::loads::Axial*) load);
					default:
						return nullptr;
				}
			}
			
			double* Bar::load_axial(double* f, const boundary::loads::Axial* load) const
			{
				//kinematics
				const double* Xi = node(0)->coordinates();
				const double* Xj = node(1)->coordinates();
				//value
				const double p = load->value();
				//length
				const double L = mat::norm(Xj, Xi, 3);
				//direction
				double s1[3];
				mat::normalize(s1, Xj, Xi, 3);
				//external force
				f[0] = p * L * s1[0] / 2;
				f[1] = p * L * s1[1] / 2;
				f[2] = p * L * s1[2] / 2;
				f[3] = p * L * s1[0] / 2;
				f[4] = p * L * s1[1] / 2;
				f[5] = p * L * s1[2] / 2;
				return f;
			}
			
			double* Bar::inertia(double* m) const
			{
				//kinematics
				const double* Xi = node(0)->coordinates();
				const double* Xj = node(1)->coordinates();
				//length
				const double L = mat::norm(Xj, Xi, 3);
				//material
				const double p = material()->specific_mass();
				//section
				const double A = ((cells::Line*) cell())->section()->area();
				//mass
				mat::clean(m, 36);
				m[0 + 6 * 0] = m[1 + 6 * 1] = m[2 + 6 * 2] = m[3 + 6 * 3] = m[4 + 6 * 4] = m[5 + 6 * 5] = p * A * L / 3;
				m[1 + 6 * 4] = m[2 + 6 * 5] = m[3 + 6 * 0] = m[4 + 6 * 1] = m[5 + 6 * 2] = m[0 + 6 * 3] = p * A * L / 6;
				return m;
			}
			double* Bar::damping(double* c) const
			{
				return (double*) memset(c, 0, 36 * sizeof(double));
			}
			double* Bar::stiffness(double* k) const
			{
				//kinematics
				double pi[3], pj[3];
				const double* xi = m_geometric ? node(0)->position(pi) : node(0)->coordinates();
				const double* xj = m_geometric ? node(1)->position(pj) : node(1)->coordinates();
				//length
				const double l = mat::norm(xj, xi, 3);
				//direction
				double t1[3];
				mat::normalize(t1, xj, xi, 3);
				//stiffness
				const double kg = m_geometric * m_f / l;
				k[0 + 6 * 0] = k[3 + 6 * 3] = +(m_k - kg) * t1[0] * t1[0] + kg;
				k[1 + 6 * 1] = k[4 + 6 * 4] = +(m_k - kg) * t1[1] * t1[1] + kg;
				k[2 + 6 * 2] = k[5 + 6 * 5] = +(m_k - kg) * t1[2] * t1[2] + kg;
				k[3 + 6 * 0] = k[0 + 6 * 3] = -(m_k - kg) * t1[0] * t1[0] - kg;
				k[4 + 6 * 1] = k[1 + 6 * 4] = -(m_k - kg) * t1[1] * t1[1] - kg;
				k[5 + 6 * 2] = k[2 + 6 * 5] = -(m_k - kg) * t1[2] * t1[2] - kg;
				k[1 + 6 * 0] = k[0 + 6 * 1] = k[4 + 6 * 3] = k[3 + 6 * 4] = +(m_k - kg) * t1[0] * t1[1];
				k[2 + 6 * 0] = k[0 + 6 * 2] = k[5 + 6 * 3] = k[3 + 6 * 5] = +(m_k - kg) * t1[0] * t1[2];
				k[2 + 6 * 1] = k[1 + 6 * 2] = k[5 + 6 * 4] = k[4 + 6 * 5] = +(m_k - kg) * t1[1] * t1[2];
				k[4 + 6 * 0] = k[0 + 6 * 4] = k[3 + 6 * 1] = k[1 + 6 * 3] = -(m_k - kg) * t1[0] * t1[1];
				k[5 + 6 * 0] = k[0 + 6 * 5] = k[3 + 6 * 2] = k[2 + 6 * 3] = -(m_k - kg) * t1[0] * t1[2];
				k[5 + 6 * 1] = k[1 + 6 * 5] = k[4 + 6 * 2] = k[2 + 6 * 4] = -(m_k - kg) * t1[1] * t1[2];
				//return
				return k;
			}
			
			bool Bar::m_strain = false;
		}
	}
}
