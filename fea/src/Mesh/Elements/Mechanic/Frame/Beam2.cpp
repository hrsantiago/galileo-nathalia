//std
#include <cmath>
#include <cstring>

//mat
#include "misc/util.h"
#include "misc/stress.h"
#include "linear/dense.h"
#include "linear/linear.h"
#include "linear/vector.h"

//fea
#include "Mesh/Nodes/Dofs.h"
#include "Mesh/Nodes/Node.h"

#include "Mesh/Cells/Types.h"
#include "Mesh/Cells/Line/Line.h"

#include "Mesh/Materials/Mechanic/Mechanic.h"

#include "Mesh/Sections/Section.h"

#include "Mesh/Elements/Types.h"
#include "Mesh/Elements/States.h"
#include "Mesh/Elements/Mechanic/Frame/Beam2.h"

#include "Mesh/Points/Mechanic/Fiber.h"
#include "Mesh/Points/Mechanic/Section.h"
#include "Mesh/Points/Mechanic/Mechanic.h"

#include "Boundary/Loads/Types.h"
#include "Boundary/Loads/Elements/Mechanic/Frame/Axial.h"
#include "Boundary/Loads/Elements/Mechanic/Frame/Shear.h"
#include "Boundary/Loads/Elements/Mechanic/Frame/Bending.h"

namespace fea
{
	namespace mesh
	{
		namespace elements
		{
			//constructors
			Beam2::Beam2(void) : m_nr(0), m_mr(0), m_ma(0), m_tr_old(0), m_tr_new(0)
			{
				memset(m_f, 0, 3 * sizeof(double));
				memset(m_k, 0, 9 * sizeof(double));
			}

			//destructor
			Beam2::~Beam2(void)
			{
				return;
			}

			//data
			bool Beam2::shear(void)
			{
				return m_shear;
			}
			bool Beam2::shear(bool shear)
			{
				return m_shear = shear;
			}
			bool Beam2::mixed(void)
			{
				return m_mixed;
			}
			bool Beam2::mixed(bool mixed)
			{
				return m_mixed = mixed;
			}
			
			double Beam2::residual_force(void) const
			{
				return m_nr;
			}
			double Beam2::residual_force(double nr)
			{
				return m_nr = nr;
			}
			double Beam2::residual_moment(bool s) const
			{
				return s ? m_mr : m_ma;
			}
			double Beam2::residual_moment(bool s, double m)
			{
				return (s ? m_mr : m_ma) = m;
			}

			//types
			unsigned Beam2::cells(void) const
			{
				return (unsigned) cells::type::beam;
			}
			unsigned Beam2::loads(void) const
			{
				return
					(unsigned) boundary::loads::type::axial |
					(unsigned) boundary::loads::type::shear |
					(unsigned) boundary::loads::type::bending;
			}
			unsigned Beam2::states(void) const
			{
				return 
					(unsigned) elements::state::axial |
					(unsigned) elements::state::shear_y |
					(unsigned) elements::state::bending_z;
			}
			unsigned Beam2::stresses(void) const
			{
				return !m_shear ?
					(unsigned) mat::stress::sxx :
					(unsigned) mat::stress::sxx |
					(unsigned) mat::stress::sxy;
			}
			unsigned Beam2::dofs(unsigned) const
			{
				return 
					(unsigned) nodes::dof::translation_x |
					(unsigned) nodes::dof::translation_y |
					(unsigned) nodes::dof::rotation_z;
			}
			elements::type Beam2::type(void) const
			{
				return elements::type::beam2;
			}

			//analysis
			void Beam2::apply(void)
			{
				//kinematics
				double xi[3], xj[3];
				node(0)->position(xi);
				node(1)->position(xj);
				const double* Xi = node(0)->coordinates();
				const double* Xj = node(1)->coordinates();
				//directions
				double s1[3], t1[3];
				mat::normalize(s1, Xj, Xi, 3);
				mat::normalize(t1, xj, xi, 3);
				//lengths
				const double L = mat::norm(Xj, Xi, 3);
				const double l = mat::norm(xj, xi, 3);
				//rotations
				const double ti = node(0)->state(nodes::dof::rotation_z);
				const double tj = node(1)->state(nodes::dof::rotation_z);
				//displacements
				const double ui = node(0)->state(nodes::dof::translation_x);
				const double vi = node(0)->state(nodes::dof::translation_y);
				const double uj = node(1)->state(nodes::dof::translation_x);
				const double vj = node(1)->state(nodes::dof::translation_y);
				//rigid rotation
				const double tr_old = m_tr_old;
				const double cr_old = cos(tr_old);
				const double sr_old = sin(tr_old);
				const double cr_new = t1[0] * s1[0] + t1[1] * s1[1];
				const double sr_new = t1[1] * s1[0] - t1[0] * s1[1];
				const double cr_inc = cr_new * cr_old + sr_new * sr_old;
				const double sr_inc = sr_new * cr_old - cr_new * sr_old;
				const double tr_new = m_tr_new = tr_old + atan(sr_inc / cr_inc);
				//local displacements
				const double u[] = {
					m_geometric ? l - L : s1[0] * (uj - ui) + s1[1] * (vj - vi),
					m_geometric ? ti - tr_new : ti - s1[0] * (vj - vi) / L + s1[1] * (uj - ui) / L,
					m_geometric ? tj - tr_new : tj - s1[0] * (vj - vi) / L + s1[1] * (uj - ui) / L
				};
				//material
				const double n = ((materials::Mechanic*) material(0))->poisson_ratio();
				const double G = ((materials::Mechanic*) material(0))->shear_modulus();
				const double E = ((materials::Mechanic*) material(0))->elastic_modulus();
				//section
				const double A = ((cells::Line*) cell())->section()->area();
				const double I = ((cells::Line*) cell())->section()->inertia_z();
				const double k = ((cells::Line*) cell())->section()->shear_coefficient_y(n);
				//shear parameters
				const double q = k * G * A * L * L;
				const double w = !m_shear ? 0 : E * I / q;
				const double m = !m_shear ? 1 : 1 / (1 + 12 * w);
				//local formulation
				m_inelastic ? m_mixed ? local_mixed(u, L, A, I) : local_fiber(u, L, A, I, w, m) : local_elastic(u, L, A, I, w, m);
			}
			
			void Beam2::record(void)
			{
				//length
				double xi[3], xj[3];
				node(0)->position(xi);
				node(1)->position(xj);
				const double l = mat::norm(xj, xi, 3);
				//states
				const double nix = +m_f[0];
				const double njx = +m_f[0];
				const double miz = -m_f[1];
				const double mjz = +m_f[2];
				const double niy = -(miz + mjz) / l;
				const double njy = -(miz + mjz) / l;
				//record
				char formatter[200];
				sprintf(formatter, "%+.6e %+.6e %+.6e %+.6e %+.6e %+.6e\n", nix, niy, miz, njx, njy, mjz);
				m_results += formatter;
			}
			void Beam2::update(void)
			{
				m_tr_old = m_tr_new;
			}
			void Beam2::restore(void)
			{
				m_tr_new = m_tr_old;
			}

			//formulation
			double* Beam2::internal_force(double* f) const
			{
				//kinematics
				double pi[3], pj[3];
				const double* xi = m_geometric ? node(0)->position(pi) : node(0)->coordinates();
				const double* xj = m_geometric ? node(1)->position(pj) : node(1)->coordinates();
				//length
				const double l = mat::norm(xj, xi, 3);
				//local system
				const double c = (xj[0] - xi[0]) / l;
				const double s = (xj[1] - xi[1]) / l;
				//local force
				const double nljx = m_f[0];
				const double mliz = m_f[1];
				const double mljz = m_f[2];
				const double nljy = -(mliz + mljz) / l;
				//internal force
				f[2] = mliz;
				f[5] = mljz;
				f[0] = -c * nljx + s * nljy;
				f[1] = -s * nljx - c * nljy;
				f[3] = +c * nljx - s * nljy;
				f[4] = +s * nljx + c * nljy;
				return f;
			}
			double* Beam2::inertial_force(double* f) const
			{
				double a[6], m[36];
				return mat::multiply(f, inertia(m), acceleration(a), 6, 6);
			}
			
			double* Beam2::reference_force(double* f, const boundary::loads::Element* load) const
			{
				switch(load->type())
				{
					case boundary::loads::type::axial:
						return load_axial(f, (const boundary::loads::Axial*) load);
					case boundary::loads::type::shear:
						return load_shear(f, (const boundary::loads::Shear*) load);
					case boundary::loads::type::bending:
						return load_bending(f, (const boundary::loads::Bending*) load);
					default:
						return nullptr;
				}
			}
			double* Beam2::load_axial(double* f, const boundary::loads::Axial* load) const
			{
				//value
				const double p = load->value();
				//coordinates
				const double* Xi = node(0)->coordinates();
				const double* Xj = node(1)->coordinates();
				//length
				const double L = mat::norm(Xj, Xi, 3);
				//directions
				const double c = (Xj[0] - Xi[0]) / L;
				const double s = (Xj[1] - Xi[1]) / L;
				//force
				f[2] = 0;
				f[5] = 0;
				f[0] = p * L * c / 2;
				f[1] = p * L * s / 2;
				f[3] = p * L * c / 2;
				f[4] = p * L * s / 2;
				return f;
			}
			double* Beam2::load_shear(double* f, const boundary::loads::Shear* load) const
			{
				//value
				const double p = load->value();
				//coordinates
				const double* Xi = node(0)->coordinates();
				const double* Xj = node(1)->coordinates();
				//length
				const double L = mat::norm(Xj, Xi, 3);
				//directions
				const double c = (Xj[0] - Xi[0]) / L;
				const double s = (Xj[1] - Xi[1]) / L;
				//force
				f[0] = -p * L * s / 2;
				f[1] = +p * L * c / 2;
				f[3] = -p * L * s / 2;
				f[4] = +p * L * c / 2;
				f[2] = +p * L * L / 12;
				f[5] = -p * L * L / 12;
				return f;
			}
			double* Beam2::load_bending(double* f, const boundary::loads::Bending* load) const
			{
				//value
				const double p = load->value();
				//coordinates
				const double* Xi = node(0)->coordinates();
				const double* Xj = node(1)->coordinates();
				//length
				const double L = mat::norm(Xj, Xi, 3);
				//directions
				const double c = (Xj[0] - Xi[0]) / L;
				const double s = (Xj[1] - Xi[1]) / L;
				//force
				f[2] = +0;
				f[5] = +0;
				f[0] = +p * s;
				f[1] = -p * c;
				f[3] = -p * s;
				f[4] = +p * c;
				return f;
			}
			
			double* Beam2::inertia(double* m) const
			{
				//kinematics
				double pi[3], pj[3];
				const double* xi = m_geometric ? node(0)->position(pi) : node(0)->coordinates();
				const double* xj = m_geometric ? node(1)->position(pj) : node(1)->coordinates();
				//length
				const double l = mat::norm(xj, xi, 3);
				//direction
				const double c = (xj[0] - xi[0]) / l;
				const double s = (xj[1] - xi[1]) / l;
				const double T[] = 
				{
					+c, +s, +0, +0, +0, +0,
					-s, +c, +0, +0, +0, +0,
					+0, +0, +1, +0, +0, +0,
					+0, +0, +0, +c, +s, +0,
					+0, +0, +0, -s, +c, +0,
					+0, +0, +1, +0, +0, +1
				};
				//material
				const double r = material()->specific_mass();
				//section
				const double A = ((cells::Line*) cell())->section()->area();
				const double I = ((cells::Line*) cell())->section()->inertia_z();
				//inertia
				double ml[36];
				mat::clean(ml, 36);
				ml[6 * 0 + 0] = ml[6 * 3 + 3] = +r * A * l / 3;
				ml[6 * 0 + 3] = ml[6 * 3 + 0] = +r * A * l / 6;
				ml[6 * 1 + 1] = ml[6 * 4 + 4] = +r * (156 * A * l + 504 * I / l) / 420;
				ml[6 * 2 + 2] = ml[6 * 5 + 5] = +r * (A * l * l * l + 14 * I * l) / 105;
				ml[6 * 2 + 1] = ml[6 * 1 + 2] = +r * (11 * A * l * l + 21 * I) / 210;
				ml[6 * 5 + 4] = ml[6 * 4 + 5] = -r * (11 * A * l * l + 21 * I) / 210;
				ml[6 * 4 + 1] = ml[6 * 1 + 4] = +r * (54 * A * l - 504 * I / l) / 420;
				ml[6 * 5 + 1] = ml[6 * 1 + 5] = -r * (13 * A * l * l - 42 * I) / 420;
				ml[6 * 4 + 2] = ml[6 * 2 + 4] = +r * (13 * A * l * l - 42 * I) / 420;
				ml[6 * 5 + 2] = ml[6 * 2 + 5] = -r * (3 * A * l * l * l + 14 * I * l) / 420;
				return mat::multisym(m, T, ml, 6, 6);
			}
			double* Beam2::damping(double* c) const
			{
				return (double*) memset(c, 0, 36 * sizeof(double));
			}
			double* Beam2::stiffness(double* k) const
			{
				//kinematics
				double pi[3], pj[3];
				const double* xi = m_geometric ? node(0)->position(pi) : node(0)->coordinates();
				const double* xj = m_geometric ? node(1)->position(pj) : node(1)->coordinates();
				//length
				const double l = mat::norm(xj, xi, 3);
				//direction
				const double c = (xj[0] - xi[0]) / l;
				const double s = (xj[1] - xi[1]) / l;
				//local force
				const double nljx = m_f[0];
				const double mliz = m_f[1];
				const double mljz = m_f[2];
				//local stiffness
				const double kl00 = m_k[0];
				const double kl11 = m_k[4];
				const double kl22 = m_k[8];
				const double kl10 = m_k[1];
				const double kl20 = m_k[2];
				const double kl21 = m_k[5];
				const double klmc = m_geometric * (mliz + mljz) / (l * l) - (kl10 + kl20) / l;
				const double klnc = m_geometric * nljx / l + (kl11 + kl22 + 2 * kl21) / (l * l);
				//stiffness
				k[2 + 6 * 2] = kl11;
				k[5 + 6 * 5] = kl22;
				k[5 + 6 * 2] = k[2 + 6 * 5] = kl21;
				k[0 + 6 * 2] = k[2 + 6 * 0] = -c * kl10 - s * (kl11 + kl21) / l;
				k[3 + 6 * 2] = k[2 + 6 * 3] = +c * kl10 + s * (kl11 + kl21) / l;
				k[0 + 6 * 5] = k[5 + 6 * 0] = -c * kl20 - s * (kl21 + kl22) / l;
				k[3 + 6 * 5] = k[5 + 6 * 3] = +c * kl20 + s * (kl21 + kl22) / l;
				k[1 + 6 * 2] = k[2 + 6 * 1] = -s * kl10 + c * (kl11 + kl21) / l;
				k[4 + 6 * 2] = k[2 + 6 * 4] = +s * kl10 - c * (kl11 + kl21) / l;
				k[1 + 6 * 5] = k[5 + 6 * 1] = -s * kl20 + c * (kl21 + kl22) / l;
				k[4 + 6 * 5] = k[5 + 6 * 4] = +s * kl20 - c * (kl21 + kl22) / l;
				k[0 + 6 * 0] = k[3 + 6 * 3] = +c * c * kl00 + s * s * klnc - 2 * c * s * klmc;
				k[0 + 6 * 3] = k[3 + 6 * 0] = -c * c * kl00 - s * s * klnc + 2 * c * s * klmc;
				k[1 + 6 * 1] = k[4 + 6 * 4] = +s * s * kl00 + c * c * klnc + 2 * c * s * klmc;
				k[1 + 6 * 4] = k[4 + 6 * 1] = -s * s * kl00 - c * c * klnc - 2 * c * s * klmc;
				k[1 + 6 * 0] = k[0 + 6 * 1] = k[4 + 6 * 3] = k[3 + 6 * 4] = +c * s * (kl00 - klnc) + (c * c - s * s) * klmc;
				k[4 + 6 * 0] = k[0 + 6 * 4] = k[1 + 6 * 3] = k[3 + 6 * 1] = -c * s * (kl00 - klnc) - (c * c - s * s) * klmc;
				//return
				return k;
			}
			
			void Beam2::local_mixed(const double* u, double L, double A, double I)
			{
				return;
			}
			void Beam2::local_fiber(const double* u, double L, double A, double I, double w, double m)
			{
				//data
				const unsigned r = cell()->points();
				const unsigned n = m_shear ? 2 : 1;
				double x[r], p[r], e[2], s[2], sr[2], B[6], C[4];
				//matrices
				mat::vector sm(s, n), fm(m_f, 3);
				mat::matrix Bm(B, 3, n), Cm(C, n, n), Km(m_k, 3, 3);
				//points
				mat::clean(m_f, 3);
				mat::clean(m_k, 9);
//				mat::gauss_legendre(x, p, m);
				for(unsigned i = 0; i < r; i++)
				{
					//fibers
					for(points::Fiber& fiber : ((points::Section*) m_points[i])->m_fibers)
					{
						//fiber data
						const double a = fiber.m_area;
						const double y = fiber.m_position_y;
						//kinematic matrix
						B[0] = 1 / L;										B[3] = 0;
						B[1] = y / L * m * (4 + 12 * w - 3 * (x[i] + 1));	B[4] = -6 * m * w;
						B[2] = y / L * m * (2 - 12 * w - 3 * (x[i] + 1));	B[5] = -6 * m * w;
						//residual stress
						sr[1] = -2 * m_ma / L / A;
						sr[0] = m_nr / A - (m_mr + m_ma * x[i]) * y / I;
						//strain
						e[0] = mat::dot(B + 0, u, 3);
						e[1] = mat::dot(B + 3, u, 3);
						//return mapping
						((materials::Mechanic*) material())->return_mapping(s, C, e, sr, fiber.m_point);
						//internal force
						fm += L * p[i] * a / 2 * Bm * sm;
						//stiffness
						Km += L * p[i] * a / 2 * Cm * Bm * Cm.transpose();
					}
				}
			}
			void Beam2::local_elastic(const double* u, double L, double A, double I, double w, double m)
			{
				//material
				const double E = ((materials::Mechanic*) material(0))->elastic_modulus();
				//stiffness parameters
				const double ka = E * A / L;
				const double k1 = (!m_mixed ? m * m * (1 + 15 * w + 36 * w * w) : (1 + 3 * w) * m) * 4 * E * I / L;
				const double k2 = (!m_mixed ? m * m * (1 -  6 * w - 72 * w * w) : (1 - 6 * w) * m) * 2 * E * I / L;
				//local force
				m_f[0] = ka * u[0] + m_nr;
				m_f[1] = k1 * u[1] + k2 * u[2] - m_mr + m_ma;
				m_f[2] = k2 * u[1] + k1 * u[2] + m_mr + m_ma;
				//local stiffness
				m_k[0] = ka;
				m_k[4] = m_k[8] = k1;
				m_k[5] = m_k[7] = k2;
				m_k[1] = m_k[2] = m_k[3] = m_k[6] = 0;
			}

			//static attributes
			bool Beam2::m_shear = false;
			bool Beam2::m_mixed = false;
		}
	}
}
