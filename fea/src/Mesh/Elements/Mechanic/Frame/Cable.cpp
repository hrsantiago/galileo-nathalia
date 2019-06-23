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
#include "Mesh/Elements/Mechanic/Frame/Cable.h"

#include "Mesh/Points/Mechanic/Fiber.h"
#include "Mesh/Points/Mechanic/Section.h"
#include "Mesh/Points/Mechanic/Mechanic.h"

namespace fea
{
	namespace mesh
	{
		namespace elements
		{
			//constructors
			Cable::Cable(void) : m_f(0), m_k(0), m_sr(0)
			{
				return;
			}

			//destructor
			Cable::~Cable(void)
			{
				return;
			}
			
			//serialization
			void Cable::load(FILE* file)
			{
				Frame::load(file);
				fscanf(file, "%lf", &m_sr);
			}
			void Cable::save(FILE* file) const
			{
				Frame::save(file);
				fprintf(file, "%+.6e ", m_sr);
			}

			//types
			unsigned Cable::cells(void) const
			{
				return (unsigned) cells::type::bar;
			}
			unsigned Cable::loads(void) const
			{
				return 0;
			}
			unsigned Cable::states(void) const
			{
				return (unsigned) elements::state::axial;
			}
			unsigned Cable::stresses(void) const
			{
				return (unsigned) mat::stress::sxx;
			}
			unsigned Cable::dofs(unsigned) const
			{
				return 
					(unsigned) nodes::dof::translation_x |
					(unsigned) nodes::dof::translation_y |
					(unsigned) nodes::dof::translation_z;
			}
			elements::type Cable::type(void) const
			{
				return elements::type::bar;
			}

			//data
			bool Cable::strain(void)
			{
				return m_strain;
			}
			bool Cable::strain(bool strain)
			{
				return m_strain = strain;
			}
			
			double Cable::residual_stress(void) const
			{
				return m_sr;
			}
			double Cable::residual_stress(double sr)
			{
				return m_sr = sr;
			}

			//analysis
			void Cable::apply(void)
			{
				//length
				double pi[3], pj[3];
				double L = 0, l = 0;
				for(unsigned i = 1; i < m_nodes.size(); i++)
				{
					const double* xi = node(i - 1)->position(pi);
					const double* xj = node(i + 0)->position(pj);
					const double* Xi = node(i - 1)->coordinates();
					const double* Xj = node(i + 0)->coordinates();
					for(unsigned j = 0; j < 3; j++)
					{
						l += (xj[j] - xi[j]) * (xj[j] - xi[j]);
						L += (Xj[j] - Xi[j]) * (Xj[j] - Xi[j]);
					}
				}
				l = sqrt(l);
				L = sqrt(L);
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
				const double e = m_strain ? el : eg;
				//return mapping
				double s, C = E;
				if(!m_inelastic)
				{
					s = E * e + m_sr;
				}
				else
				{
					((materials::Mechanic*) material(0))->return_mapping(&s, &C, &e, &m_sr, mp);
				}
				//local force
				m_f = s < 0 ? 0 : m_strain ? s * A / a : a * s * A;
				//local stiffness
				const double ks = C * A / L;
				const double kg = (a * a * C + s) * A / L;
				const double kl = (C - s) / (a * a) * A / L;
				m_k = s < 0 ? 0 : !m_geometric ? ks : m_strain ? kl : kg;
			}
			
			void Cable::record(void)
			{
				char formatter[200];
				sprintf(formatter, "%+.6e %+.6e\n", m_f, m_f);
				m_results += formatter;
			}

			//formulation
			double* Cable::inertial_force(double* f) const
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
			double* Cable::internal_force(double* f) const
			{
				//data
				double pi[3], pj[3], t1[3];
				const unsigned nn = m_nodes.size();
				memset(f, 0, 3 * nn * sizeof(double));
				//nodes loop
				for(unsigned i = 1; i < nn; i++)
				{
					//kinematics
					const double* xi = m_geometric ? node(0)->position(pi) : node(0)->coordinates();
					const double* xj = m_geometric ? node(1)->position(pj) : node(1)->coordinates();
					//direction
					mat::normalize(t1, xj, xi, 3);
					//internal force
					f[3 * i - 3] += -m_f * t1[0];
					f[3 * i - 2] += -m_f * t1[1];
					f[3 * i - 1] += -m_f * t1[2];
					f[3 * i + 0] += +m_f * t1[0];
					f[3 * i + 1] += +m_f * t1[1];
					f[3 * i + 2] += +m_f * t1[2];
				}
				//return
				return f;
			}
			
			double* Cable::reference_force(double* f, const boundary::loads::Element*) const
			{
				return f;
			}
			
			double* Cable::inertia(double* m) const
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
			double* Cable::damping(double* c) const
			{
				return (double*) memset(c, 0, 36 * sizeof(double));
			}
			double* Cable::stiffness(double* k) const
			{
				//data
				double pi[3], pj[3], t1[3];
				const unsigned nn = m_nodes.size();
				memset(k, 0, 9 * nn * nn * sizeof(double));
				//nodes loop
				for(unsigned i = 1; i < nn; i++)
				{
					//kinematics
					const double* xi = m_geometric ? node(0)->position(pi) : node(0)->coordinates();
					const double* xj = m_geometric ? node(1)->position(pj) : node(1)->coordinates();
					//length
					const double l = mat::norm(xj, xi, 3);
					//direction
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
				}
				//return
				return k;
			}
			
			bool Cable::m_strain = false;
		}
	}
}
