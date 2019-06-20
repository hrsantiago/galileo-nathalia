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
#include "Mesh/Elements/Mechanic/Frame/Rope.h"

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
			Rope::Rope(void) : m_f(0), m_k(0), m_sr(0)
			{
				return;
			}

			//destructor
			Rope::~Rope(void)
			{
				return;
			}
			
			//serialization
			void Rope::load(FILE* file)
			{
				Frame::load(file);
				fscanf(file, "%lf", &m_sr);
			}
			void Rope::save(FILE* file) const
			{
				Frame::save(file);
				fprintf(file, "%+.6e ", m_sr);
			}

			//types
			unsigned Rope::cells(void) const
			{
				return (unsigned) cells::type::bar;
			}
			unsigned Rope::loads(void) const
			{
				return 0;
			}
			unsigned Rope::states(void) const
			{
				return (unsigned) elements::state::axial;
			}
			unsigned Rope::stresses(void) const
			{
				return (unsigned) mat::stress::sxx;
			}
			unsigned Rope::dofs(unsigned) const
			{
				return 
					(unsigned) nodes::dof::translation_x |
					(unsigned) nodes::dof::translation_y |
					(unsigned) nodes::dof::translation_z;
			}
			elements::type Rope::type(void) const
			{
				return elements::type::rope;
			}

			//data
			bool Rope::strain(void)
			{
				return m_strain;
			}
			bool Rope::strain(bool strain)
			{
				return m_strain = strain;
			}
			
			double Rope::residual_stress(void) const
			{
				return m_sr;
			}
			double Rope::residual_stress(double sr)
			{
				return m_sr = sr;
			}

			//analysis
			void Rope::apply(void)
			{
				//kinematics
				double L = 0;
				double l = 0;
				double xi[3], xj[3];
				for(unsigned i = 1; i < m_nodes.size(); i++)
				{
					node(0)->position(xi);
					node(1)->position(xj);
					const double* Xi = node(i - 1)->coordinates();
					const double* Xj = node(i + 0)->coordinates();
					L += mat::norm(Xj, Xi, 3);
					l += mat::norm(xj, xi, 3);
				}
				//section
				const double A = ((cells::Line*) cell())->section()->area();
				//material
				const double E = ((materials::Mechanic*) material(0))->elastic_modulus();
				//material point
				points::Mechanic& mp = ((points::Section*) m_points[0])->m_fibers[0].m_point;
				//strain
				const double a = l / L;
				const double el = log(a);
				const double eg = (a * a - 1) / 2;
				const double et = m_strain ? el : eg;
				const double de = m_strain ? 1 / a : a;
				const double dde = m_strain ? -1 / (a * a) : 1;
				//return mapping
				double s, C = E;
				if(!m_inelastic)
				{
					s = E * et + m_sr;
				}
				else
				{
					((materials::Mechanic*) material(0))->return_mapping(&s, &C, &et, &m_sr, mp);
				}
				//local force
				m_f = s < 0 ? 0 : s * A * de;
				//local stiffness
				m_k = s < 0 ? 0 : C * A / L * de * de + s * A / L * dde;
			}
			
			void Rope::record(void)
			{
				char formatter[200];
				sprintf(formatter, "%+.6e %+.6e\n", m_f, m_f);
				m_results += formatter;
			}

			//formulation
			double* Rope::inertial_force(double* f) const
			{
				//coordinates
				const double* Xi = node(0)->coordinates();
				const double* Xj = node(1)->coordinates();
				//acceleration
				double ai[3], aj[3];
				node(0)->displacement(ai, 2);
				node(1)->displacement(aj, 2);
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
			double* Rope::internal_force(double* f) const
			{
				double t1[3], xi[3], xj[3];
				const unsigned n = m_nodes.size();
				memset(f, 0, 3 * n * sizeof(double));
				for(unsigned i = 1; i < n; i++)
				{
					node(i - 1)->position(xi);
					node(i + 0)->position(xj);
					mat::normalize(t1, xj, xi, 3);
					f[3 * i - 3] -= m_f * t1[0];
					f[3 * i - 2] -= m_f * t1[1];
					f[3 * i - 1] -= m_f * t1[2];
					f[3 * i + 0] += m_f * t1[0];
					f[3 * i + 1] += m_f * t1[1];
					f[3 * i + 2] += m_f * t1[2];
				}
				return f;
			}
			
			double* Rope::reference_force(double* f, const boundary::loads::Element* load) const
			{
				return f;
			}
			
			double* Rope::inertia(double* m) const
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
			double* Rope::damping(double* c) const
			{
				return (double*) memset(c, 0, 36 * sizeof(double));
			}
			double* Rope::stiffness(double* k) const
			{
				double li, kg;
				double pi[3], pj[3], ti[3];
				double qi[3], qj[3], tj[3];
				const unsigned n = m_nodes.size();
				memset(k, 0, 9 * n * n * sizeof(double));
				for(unsigned i = 1; i < n; i++)
				{
					node(i - 1)->position(pi);
					node(i + 0)->position(pj);
					li = mat::norm(pj, pi, 3);
					mat::normalize(ti, pj, pi, 3);
					for(unsigned j = 1; j < n; j++)
					{
						kg = (i == j) * m_f / li;
						node(j - 1)->position(qi);
						node(j + 0)->position(qj);
						mat::normalize(tj, qj, qi, 3);
						for(unsigned a = 0; a < 3; a++)
						{
							for(unsigned b = 0; b < 3; b++)
							{
								k[3 * (i - 1) + a + 3 * n * (3 * (j - 1) + b)] += (m_k - kg) * ti[a] * tj[b] + (a == b) * kg;
								k[3 * (i - 1) + a + 3 * n * (3 * (j + 0) + b)] -= (m_k - kg) * ti[a] * tj[b] + (a == b) * kg;
								k[3 * (i + 0) + a + 3 * n * (3 * (j - 1) + b)] -= (m_k - kg) * ti[a] * tj[b] + (a == b) * kg;
								k[3 * (i + 0) + a + 3 * n * (3 * (j + 0) + b)] += (m_k - kg) * ti[a] * tj[b] + (a == b) * kg;
							}
						}
					}
				}
				//return
				return k;
			}
			
			bool Rope::m_strain = false;
		}
	}
}
