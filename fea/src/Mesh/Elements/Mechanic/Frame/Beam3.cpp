//std
#include <cmath>
#include <cstring>

//mat
#include "misc/util.h"
#include "misc/stress.h"
#include "linear/lin3.h"
#include "linear/vec3.h"
#include "linear/mat3.h"
#include "linear/quat.h"
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
#include "Mesh/Elements/Mechanic/Frame/Beam3.h"

#include "Mesh/Points/Mechanic/Fiber.h"
#include "Mesh/Points/Mechanic/Section.h"
#include "Mesh/Points/Mechanic/Mechanic.h"

#include "Boundary/Loads/Types.h"
#include "Boundary/Loads/Elements/Mechanic/Frame/Axial.h"
#include "Boundary/Loads/Elements/Mechanic/Frame/Shear.h"
#include "Boundary/Loads/Elements/Mechanic/Frame/Torsion.h"
#include "Boundary/Loads/Elements/Mechanic/Frame/Bending.h"

namespace fea
{
	namespace mesh
	{
		namespace elements
		{
			//constructors
			Beam3::Beam3(void)
			{
				m_orientation[0] = 0;
				m_orientation[1] = 0;
				m_orientation[2] = 1;
				memset(m_f, 0,  6 * sizeof(double));
				memset(m_k, 0, 36 * sizeof(double));
			}

			//destructor
			Beam3::~Beam3(void)
			{
				return;
			}
			
			//serialization
			void Beam3::load(FILE* file)
			{
				Frame::load(file);
				fscanf(file, "%lf %lf %lf", &m_orientation[0], &m_orientation[1], &m_orientation[2]);
			}
			void Beam3::save(FILE* file) const
			{
				Frame::save(file);
				fprintf(file, "%+.6e %+.6e %+.6e ", m_orientation[0], m_orientation[1], m_orientation[2]);
			}

			//data
			bool Beam3::shear(void)
			{
				return m_shear;
			}
			bool Beam3::shear(bool shear)
			{
				return m_shear = shear;
			}
			bool Beam3::mixed(void)
			{
				return m_mixed;
			}
			bool Beam3::mixed(bool mixed)
			{
				return m_mixed = mixed;
			}
			
			const double* Beam3::orientation(void) const
			{
				return m_orientation;
			}
			const double* Beam3::orientation(const double* orientation)
			{
				return (const double*) memcpy(m_orientation, orientation, 3 * sizeof(double));
			}
			const double* Beam3::orientation(double s1, double s2, double s3)
			{
				m_orientation[0] = s1;
				m_orientation[1] = s2;
				m_orientation[2] = s3;
				return m_orientation;
			}

			//types
			unsigned Beam3::cells(void) const
			{
				return (unsigned) cells::type::beam;
			}
			unsigned Beam3::loads(void) const
			{
				return
					(unsigned) boundary::loads::type::axial |
					(unsigned) boundary::loads::type::shear |
					(unsigned) boundary::loads::type::torsion |
					(unsigned) boundary::loads::type::bending;
			}
			unsigned Beam3::states(void) const
			{
				return 
					(unsigned) elements::state::axial |
					(unsigned) elements::state::shear_y |
					(unsigned) elements::state::shear_z |
					(unsigned) elements::state::torsion |
					(unsigned) elements::state::bending_y |
					(unsigned) elements::state::bending_z;
			}
			unsigned Beam3::stresses(void) const
			{
				return 
					(unsigned) mat::stress::sxx |
					(unsigned) mat::stress::sxy |
					(unsigned) mat::stress::sxz;
			}
			unsigned Beam3::dofs(unsigned) const
			{
				return
					(unsigned) nodes::dof::rotation_x |
					(unsigned) nodes::dof::rotation_y |
					(unsigned) nodes::dof::rotation_z |
					(unsigned) nodes::dof::translation_x |
					(unsigned) nodes::dof::translation_y |
					(unsigned) nodes::dof::translation_z;
			}
			elements::type Beam3::type(void) const
			{
				return elements::type::beam3;
			}

			//analysis
			bool Beam3::symmetric(void) const
			{
				return false;
			}
			
			void Beam3::apply(void)
			{
				//kinematics
				double pi[3], pj[3];
				const mat::vec3 xi = node(0)->position(pi);
				const mat::vec3 xj = node(1)->position(pj);
				const mat::vec3 Xi = node(0)->coordinates();
				const mat::vec3 Xj = node(1)->coordinates();
				//length
				const double L = (Xj - Xi).norm();
				const double l = (xj - xi).norm();
				//initial triad
				const mat::vec3 s1 = (Xj - Xi) / L;
				const mat::vec3 s3 = m_orientation;
				const mat::vec3 s2 = mat::cross(s3, s1);
				//nodal quaternions
				const mat::quat qi = node(0)->quaternion();
				const mat::quat qj = node(1)->quaternion();
				//nodal triad
				const mat::vec3 ri1 = qi.rotate(s1);
				const mat::vec3 ri2 = qi.rotate(s2);
				const mat::vec3 ri3 = qi.rotate(s3);
				const mat::vec3 rj1 = qj.rotate(s1);
				const mat::vec3 rj2 = qj.rotate(s2);
				const mat::vec3 rj3 = qj.rotate(s3);
				//local triad
				const mat::vec3 t1 = !m_geometric ? s1 : (xj - xi) / l;
				const mat::vec3 t2 = !m_geometric ? s2 : mat::cross(ri3 + rj3, t1).normalize();
				const mat::vec3 t3 = !m_geometric ? s3 : mat::cross(t1, t2);
				//local strech
				const double uljx = l - L;
				//local rotations
				const double tr = 1e-6;
				const double cli1 = mat::dot(t1, ri1);
				const double cli2 = mat::dot(t2, ri2);
				const double cli3 = mat::dot(t3, ri3);
				const double clj1 = mat::dot(t1, rj1);
				const double clj2 = mat::dot(t2, rj2);
				const double clj3 = mat::dot(t3, rj3);
				const double clin = cli1 + cli2 + cli3;
				const double cljn = clj1 + clj2 + clj3;
				const double tlin = acos(mat::bound((clin - 1) / 2));
				const double tljn = acos(mat::bound((cljn - 1) / 2));
				const double qlin = fabs(tlin) < tr ? 1 : tlin / sin(tlin);
				const double qljn = fabs(tljn) < tr ? 1 : tljn / sin(tljn);
				const double tlix = qlin * (mat::dot(t3, ri2) - mat::dot(t2, ri3)) / 2;
				const double tliy = qlin * (mat::dot(t1, ri3) - mat::dot(t3, ri1)) / 2;
				const double tliz = qlin * (mat::dot(t2, ri1) - mat::dot(t1, ri2)) / 2;
				const double tljx = qljn * (mat::dot(t3, rj2) - mat::dot(t2, rj3)) / 2;
				const double tljy = qljn * (mat::dot(t1, rj3) - mat::dot(t3, rj1)) / 2;
				const double tljz = qljn * (mat::dot(t2, rj1) - mat::dot(t1, rj2)) / 2;
				//local formulation
				const double u[] = {uljx, tliz, tljz, tljx - tlix, tliy, tljy};
				m_inelastic ? m_mixed ? local_mixed(u) : local_fiber(u) : local_elastic(u);
			}
			
			bool Beam3::check(void) const
			{
				if(Element::check())
				{
					double q[3], s1[3];
					const double* s3 = m_orientation;
					const double* Xi = node(0)->coordinates();
					const double* Xj = node(1)->coordinates();
					if(mat::norm(mat::cross(q, s3, mat::normalize(s1, Xi, Xj, 3)), 3) < 1e-5)
					{
						printf("\tElement %04d (beam3) has axis and orientation too close!\n", index());
						return false;
					}
					else
					{
						return true;
					}
				}
				else
				{
					return false;
				}
				
			}
			void Beam3::prepare(void)
			{
				//base call
				Element::prepare();
				//coordinates
				const mat::vec3 Xi = node(0)->coordinates();
				const mat::vec3 Xj = node(1)->coordinates();
				//orientation
				mat::vec3(m_orientation).project((Xj - Xi).normalize());
			}
			
			void Beam3::record(void)
			{
				//length
				double xi[3], xj[3];
				node(0)->position(xi);
				node(1)->position(xj);
				const double l = mat::norm(xj, xi, 3);
				//states
				const double nix = +m_f[0];
				const double njx = +m_f[0];
				const double mix = +m_f[3];
				const double mjx = +m_f[3];
				const double miz = -m_f[1];
				const double mjz = +m_f[2];
				const double miy = -m_f[4];
				const double mjy = +m_f[5];
				const double niy = -(mjz - miz) / l;
				const double njy = -(mjz - miz) / l;
				const double niz = +(mjy - miy) / l;
				const double njz = +(mjy - miy) / l;
				//record
				char formatter[200];
				const double si[] = {nix, niy, niz, mix, miy, miz};
				const double sj[] = {njx, njy, njz, mjx, mjy, mjz};
				for(unsigned i = 0; i < 2; i++)
				{
					for(unsigned j = 0; j < 6; j++)
					{
						sprintf(formatter, "%+.6e ", i == 0 ? si[j] : sj[j]);
						m_results += formatter;
					}
				}
				m_results += "\n";
			}

			//formulation
			void Beam3::local_fiber(const double* u)
			{
				//data
				mat::clean(m_f, 6);
				mat::clean(m_k, 36);
				const unsigned np = cell()->points();
				double e[3], s[3], sr[3], B[18], C[9];
				//matrices
				mat::vector sm(s, 3), fm(m_f, 6);
				mat::matrix Bm(B, 6, 3), Cm(C, 3, 3), Km(m_k, 6, 6);
				//coordinates
				const mat::vec3 Xi = node(0)->coordinates();
				const mat::vec3 Xj = node(1)->coordinates();
				//length
				const double L = (Xj - Xi).norm();
				//points
				const double* xg = mat::gauss_points(np);
				const double* wg = mat::gauss_points(np);
				for(unsigned i = 0; i < np; i++)
				{
					//fibers
					for(points::Fiber& fiber : ((points::Section*) m_points[i])->m_fibers)
					{
						//fiber data
						const double a = fiber.m_area;
						const double y = fiber.m_position_y;
						const double z = fiber.m_position_z;
						//kinematic matrix
						B[0] = 1 / L;							B[ 6] = 0;		B[12] = 0;
						B[1] = y / L * (4 - 3 * (xg[i] + 1));	B[ 7] = 0;		B[13] = 0;
						B[2] = y / L * (2 - 3 * (xg[i] + 1));	B[ 8] = 0;		B[14] = 0;
						B[3] = 0;								B[ 9] = -z / L;	B[15] = +y / L;
						B[4] = z / L * (4 - 3 * (xg[i] + 1));	B[10] = 0;		B[16] = 0;
						B[5] = z / L * (2 - 3 * (xg[i] + 1));	B[11] = 0;		B[17] = 0;
						//residual stress
						sr[0] = sr[1] = sr[2] = 0;
						//strain
						e[0] = mat::dot(B +  0, u, 6);
						e[1] = mat::dot(B +  6, u, 6);
						e[2] = mat::dot(B + 12, u, 6);
						//return mapping
						((materials::Mechanic*) material())->return_mapping(s, C, e, sr, fiber.m_point);
						//internal force
						fm += L * wg[i] * a / 2 * Bm * sm;
						//stiffness
//						Km += L * p * a / 2 * mat::mix(Bm, Cm, false);
					}
				}
			}
			void Beam3::local_mixed(const double* u)
			{
				return;
			}
			void Beam3::local_elastic(const double* u)
			{
				//coordinates
				const mat::vec3 Xi = node(0)->coordinates();
				const mat::vec3 Xj = node(1)->coordinates();
				//length
				const double L = (Xj - Xi).norm();
				//material
				const double G = ((materials::Mechanic*) material(0))->shear_modulus();
				const double E = ((materials::Mechanic*) material(0))->elastic_modulus();
				//section
				const double As = ((cells::Line*) cell())->section()->area();
				const double Ix = ((cells::Line*) cell())->section()->inertia_x();
				const double Iy = ((cells::Line*) cell())->section()->inertia_y();
				const double Iz = ((cells::Line*) cell())->section()->inertia_z();
				//stiffness parameters
				const double ka = E * As / L;
				const double kt = G * Ix / L;
				const double ky1 = 4 * E * Iy / L;
				const double ky2 = 2 * E * Iy / L;
				const double kz1 = 4 * E * Iz / L;
				const double kz2 = 2 * E * Iz / L;
				//internal force
				m_f[0] = ka * u[0];
				m_f[3] = kt * u[3];
				m_f[1] = kz1 * u[1] + kz2 * u[2];
				m_f[2] = kz2 * u[1] + kz1 * u[2];
				m_f[4] = ky1 * u[4] + ky2 * u[5];
				m_f[5] = ky2 * u[4] + ky1 * u[5];
				//stiffness
				mat::clean(m_k, 36);
				m_k[0 + 6 * 0] = ka;
				m_k[3 + 6 * 3] = kt;
				m_k[1 + 6 * 1] = m_k[2 + 6 * 2] = kz1;
				m_k[2 + 6 * 1] = m_k[1 + 6 * 2] = kz2;
				m_k[4 + 6 * 4] = m_k[5 + 6 * 5] = ky1;
				m_k[5 + 6 * 4] = m_k[4 + 6 * 5] = ky2;
			}
			
			double* Beam3::inertial_force(double* f) const
			{
				return (double*) memset(f, 0, 12 * sizeof(double));
			}
			double* Beam3::internal_force(double* f) const
			{
				//kinematics
				double pi[3], pj[3];
				const mat::vec3 xi = node(0)->position(pi);
				const mat::vec3 xj = node(1)->position(pj);
				const mat::vec3 Xi = node(0)->coordinates();
				const mat::vec3 Xj = node(1)->coordinates();
				//length
				const double L = (Xj - Xi).norm();
				const double l = (xj - xi).norm();
				//initial triad
				const mat::vec3 s1 = (Xj - Xi) / L;
				const mat::vec3 s3 = m_orientation;
				const mat::vec3 s2 = mat::cross(s3, s1);
				//nodal quaternions
				const mat::quat qi = node(0)->quaternion();
				const mat::quat qj = node(1)->quaternion();
				//nodal triad
				const mat::vec3 ri3 = qi.rotate(s3);
				const mat::vec3 rj3 = qj.rotate(s3);
				//local triad
				const mat::vec3 t1 = !m_geometric ? s1 : (xj - xi) / l;
				const mat::vec3 t2 = !m_geometric ? s2 : mat::cross(ri3 + rj3, t1).normalize();
				const mat::vec3 t3 = !m_geometric ? s3 : mat::cross(t1, t2);
				//local	force
				const double nljx = m_f[0];
				const double mliz = m_f[1];
				const double mljz = m_f[2];
				const double mljx = m_f[3];
				const double mliy = m_f[4];
				const double mljy = m_f[5];
				const double nljy = -(mliz + mljz) / l;
				const double nljz = +(mliy + mljy) / l;
				//internal force
				f[ 0] = -nljx * t1[0] - nljy * t2[0] - nljz * t3[0];
				f[ 1] = -nljx * t1[1] - nljy * t2[1] - nljz * t3[1];
				f[ 2] = -nljx * t1[2] - nljy * t2[2] - nljz * t3[2];
				f[ 3] = -mljx * t1[0] + mliy * t2[0] + mliz * t3[0];
				f[ 4] = -mljx * t1[1] + mliy * t2[1] + mliz * t3[1];
				f[ 5] = -mljx * t1[2] + mliy * t2[2] + mliz * t3[2];
				f[ 6] = +nljx * t1[0] + nljy * t2[0] + nljz * t3[0];
				f[ 7] = +nljx * t1[1] + nljy * t2[1] + nljz * t3[1];
				f[ 8] = +nljx * t1[2] + nljy * t2[2] + nljz * t3[2];
				f[ 9] = +mljx * t1[0] + mljy * t2[0] + mljz * t3[0];
				f[10] = +mljx * t1[1] + mljy * t2[1] + mljz * t3[1];
				f[11] = +mljx * t1[2] + mljy * t2[2] + mljz * t3[2];
				return f;
			}
			double* Beam3::reference_force(double* f, const boundary::loads::Element* load) const
			{
				switch(load->type())
				{
					case boundary::loads::type::axial: 
						return load_axial(f, (const boundary::loads::Axial*) load);
					case boundary::loads::type::shear: 
						return load_shear(f, (const boundary::loads::Shear*) load);
					case boundary::loads::type::torsion: 
						return load_torsion(f, (const boundary::loads::Torsion*) load);
					case boundary::loads::type::bending: 
						return load_bending(f, (const boundary::loads::Bending*) load);
					default:
						return nullptr;
				}
			}
			
			double* Beam3::load_axial(double* f, const boundary::loads::Axial* load) const
			{
				//value
				const double p = load->value();
				//coordinates
				const double* Xi = node(0)->coordinates();
				const double* Xj = node(1)->coordinates();
				//length
				const double L = mat::norm(Xj, Xi, 3);
				//directions
				const double cx = (Xj[0] - Xi[0]) / L;
				const double cy = (Xj[1] - Xi[1]) / L;
				const double cz = (Xj[2] - Xi[2]) / L;
				//force
				f[ 3] = 0;
				f[ 4] = 0;
				f[ 5] = 0;
				f[ 9] = 0;
				f[10] = 0;
				f[11] = 0;
				f[ 0] = p * L * cx / 2;
				f[ 1] = p * L * cy / 2;
				f[ 2] = p * L * cz / 2;
				f[ 6] = p * L * cx / 2;
				f[ 7] = p * L * cy / 2;
				f[ 8] = p * L * cz / 2;
				return f;
			}
			double* Beam3::load_shear(double* f, const boundary::loads::Shear* load) const
			{
				//load
				const double p = load->value();
				const unsigned d = load->axis();
				//coordinates
				const double* Xi = node(0)->coordinates();
				const double* Xj = node(1)->coordinates();
				//length
				const double L = mat::norm(Xj, Xi, 3);
				//directions
				double t1[3], t2[3];
				const double* t3 = m_orientation;
				mat::cross(t2, t3, mat::normalize(t1, Xi, Xj, 3));
				//force
				f[ 0] = p * L / 2 * (d == 0 ? t2[0] : t3[0]);
				f[ 1] = p * L / 2 * (d == 0 ? t2[1] : t3[1]);
				f[ 2] = p * L / 2 * (d == 0 ? t2[2] : t3[2]);
				f[ 6] = p * L / 2 * (d == 0 ? t2[0] : t3[0]);
				f[ 7] = p * L / 2 * (d == 0 ? t2[1] : t3[1]);
				f[ 8] = p * L / 2 * (d == 0 ? t2[2] : t3[2]);
				f[ 3] = p * L * L / 12 * (d == 0 ? t2[0] : t3[0]);
				f[ 4] = p * L * L / 12 * (d == 0 ? t2[0] : t3[0]);
				f[ 5] = p * L * L / 12 * (d == 0 ? t2[0] : t3[0]);
				f[ 9] = p * L * L / 12 * (d == 0 ? t2[0] : t3[0]);
				f[10] = p * L * L / 12 * (d == 0 ? t2[0] : t3[0]);
				f[11] = p * L * L / 12 * (d == 0 ? t2[0] : t3[0]);
				return f;
			}
			double* Beam3::load_torsion(double* f, const boundary::loads::Torsion* load) const
			{
				//value
				const double p = load->value();
				//coordinates
				const double* Xi = node(0)->coordinates();
				const double* Xj = node(1)->coordinates();
				//length
				const double L = mat::norm(Xj, Xi, 3);
				//directions
				const double cx = (Xj[0] - Xi[0]) / L;
				const double cy = (Xj[1] - Xi[1]) / L;
				const double cz = (Xj[2] - Xi[2]) / L;
				//force
				f[ 0] = 0;
				f[ 1] = 0;
				f[ 2] = 0;
				f[ 6] = 0;
				f[ 7] = 0;
				f[ 8] = 0;
				f[ 3] = p * L * cx / 2;
				f[ 4] = p * L * cy / 2;
				f[ 5] = p * L * cz / 2;
				f[ 9] = p * L * cx / 2;
				f[10] = p * L * cy / 2;
				f[11] = p * L * cz / 2;
				return f;
			}
			double* Beam3::load_bending(double* f, const boundary::loads::Bending* load) const
			{
				return (double*) memset(f, 0, 12 * sizeof(double));
			}
			
			double* Beam3::inertia(double* m) const
			{
				//kinematics
				double pi[3], pj[3];
				const mat::vec3 xi = node(0)->position(pi);
				const mat::vec3 xj = node(1)->position(pj);
				const mat::vec3 Xi = node(0)->coordinates();
				const mat::vec3 Xj = node(1)->coordinates();
				//length
				const double L = (Xj - Xi).norm();
				const double l = (xj - xi).norm();
				//global triad
				const mat::vec3 e1 = {1, 0, 0};
				const mat::vec3 e2 = {0, 1, 0};
				const mat::vec3 e3 = {0, 0, 1};
				//initial triad
				const mat::vec3 s1 = (Xj - Xi) / L;
				const mat::vec3 s3 = m_orientation;
				const mat::vec3 s2 = mat::cross(s3, s1);
				//nodal quaternions
				const mat::quat qi = node(0)->quaternion();
				const mat::quat qj = node(1)->quaternion();
				//nodal triad
				const mat::vec3 ri3 = qi.rotate(s3);
				const mat::vec3 rj3 = qj.rotate(s3);
				//local triad
				const mat::vec3 t1 = !m_geometric ? s1 : (xj - xi) / l;
				const mat::vec3 t2 = !m_geometric ? s2 : mat::cross(ri3 + rj3, t1).normalize();
				const mat::vec3 t3 = !m_geometric ? s3 : mat::cross(t1, t2);
				//material
				const double r = material()->specific_mass();
				//section
				const double A = ((cells::Line*) cell())->section()->area();
				const double Ix = ((cells::Line*) cell())->section()->inertia_x();
				const double Iy = ((cells::Line*) cell())->section()->inertia_y();
				const double Iz = ((cells::Line*) cell())->section()->inertia_z();
				//inertia
				double ml[144];
				mat::clean(ml, 144);
				ml[ 0 + 12 *  0] = ml[ 6 + 12 *  6] = r * A * l / 3;
				ml[ 0 + 12 *  6] = ml[ 6 + 12 *  0] = r * A * l / 6;
				ml[ 3 + 12 *  3] = ml[ 9 + 12 *  9] = r * Ix * l / 3;
				ml[ 3 + 12 *  9] = ml[ 9 + 12 *  3] = r * Ix * l / 6;
				ml[ 1 + 12 *  7] = ml[ 7 + 12 *  1] = r * (+9 * A * l - 84 * Iz / l) / 70;
				ml[ 2 + 12 *  8] = ml[ 8 + 12 *  2] = r * (+9 * A * l - 84 * Iy / l) / 70;
				ml[ 1 + 12 *  1] = ml[ 7 + 12 *  7] = r * (+13 * A * l + 42 * Iz / l) / 35;
				ml[ 2 + 12 *  2] = ml[ 8 + 12 *  8] = r * (+13 * A * l + 42 * Iy / l) / 35;
				ml[ 1 + 12 *  5] = ml[ 5 + 12 *  1] = r * (+11 * A * l * l + 21 * Iz) / 210;
				ml[ 1 + 12 * 11] = ml[11 + 12 *  1] = r * (-13 * A * l * l + 42 * Iz) / 420;
				ml[ 2 + 12 *  4] = ml[ 4 + 12 *  2] = r * (-11 * A * l * l - 21 * Iy) / 210;
				ml[ 2 + 12 * 10] = ml[10 + 12 *  2] = r * (+13 * A * l * l - 42 * Iy) / 420;
				ml[ 4 + 12 *  8] = ml[ 8 + 12 *  4] = r * (-13 * A * l * l + 42 * Iy) / 420;
				ml[ 5 + 12 *  7] = ml[ 7 + 12 *  5] = r * (+13 * A * l * l - 42 * Iz) / 420;
				ml[ 7 + 12 * 11] = ml[11 + 12 *  7] = r * (-11 * A * l * l - 21 * Iz) / 210;
				ml[ 8 + 12 * 10] = ml[10 + 12 *  8] = r * (+11 * A * l * l + 21 * Iy) / 210;
				ml[ 4 + 12 *  4] = ml[10 + 12 * 10] = r * (A * l * l * l + 14 * Iy * l) / 105;
				ml[ 5 + 12 *  5] = ml[11 + 12 * 11] = r * (A * l * l * l + 14 * Iz * l) / 105;
				ml[ 4 + 12 * 10] = ml[10 + 12 *  4] = r * (-3 * A * l * l * l - 14 * Iy * l) / 420;
				ml[ 5 + 12 * 11] = ml[11 + 12 *  5] = r * (-3 * A * l * l * l - 14 * Iz * l) / 420;
				//rotation
				double G[144];
				mat::clean(G, 144);
				const mat::mat3 R = t1.outer(e1) + t2.outer(e2) + t3.outer(e3);
				for(unsigned i = 0; i < 3; i++)
				{
					for(unsigned j = 0; j < 3; j++)
					{
						for(unsigned k = 0; k < 4; k++)
						{
							G[39 * k + 12 * j + i] = R(i, j);
						}
					}
				}
				return mat::multisym(m, G, ml, 12, 12);
			}
			double* Beam3::damping(double* c) const
			{
				return (double*) memset(c, 0, 144 * sizeof(double));
			}
			double* Beam3::stiffness(double* k) const
			{
				//kinematics
				double pi[3], pj[3];
				const mat::vec3 xi = node(0)->position(pi);
				const mat::vec3 xj = node(1)->position(pj);
				const mat::vec3 Xi = node(0)->coordinates();
				const mat::vec3 Xj = node(1)->coordinates();
				//length
				const double L = (Xj - Xi).norm();
				const double l = (xj - xi).norm();
				//initial triad
				const mat::vec3 s1 = (Xj - Xi) / L;
				const mat::vec3 s3 = m_orientation;
				const mat::vec3 s2 = mat::cross(s3, s1);
				//nodal quaternions
				const mat::quat qi = node(0)->quaternion();
				const mat::quat qj = node(1)->quaternion();
				//nodal triad
				const mat::vec3 ri3 = qi.rotate(s3);
				const mat::vec3 rj3 = qj.rotate(s3);
				//local triad
				const mat::vec3 t1 = !m_geometric ? s1 : (xj - xi) / l;
				const mat::vec3 t2 = !m_geometric ? s2 : mat::cross(ri3 + rj3, t1).normalize();
				const mat::vec3 t3 = !m_geometric ? s3 : mat::cross(t1, t2);
				//local system
				const double T[72] = {
					-t1[0], +t2[0] / l, +t2[0] / l, +0, 	-t3[0] / l, -t3[0] / l,
					-t1[1], +t2[1] / l, +t2[1] / l, +0,		-t3[1] / l, -t3[1] / l,
					-t1[2], +t2[2] / l, +t2[2] / l, +0, 	-t3[2] / l, -t3[2] / l,
					+0, 	+t3[0], 	+0, 		-t1[0], +t2[0], 	+0,
					+0, 	+t3[1], 	+0, 		-t1[1], +t2[1], 	+0,
					+0, 	+t3[2], 	+0, 		-t1[2], +t2[2], 	+0,
					+t1[0], -t2[0] / l, -t2[0] / l, +0, 	+t3[0] / l, +t3[0] / l,
					+t1[1], -t2[1] / l, -t2[1] / l, +0, 	+t3[1] / l, +t3[1] / l,
					+t1[2], -t2[2] / l, -t2[2] / l, +0, 	+t3[2] / l, +t3[2] / l,
					+0, 	+0, 		+t3[0], 	+t1[0], +0, 		+t2[0],
					+0, 	+0, 		+t3[1], 	+t1[1], +0, 		+t2[1],
					+0, 	+0, 		+t3[2], 	+t1[2], +0, 		+t2[2]
				};
				//material stiffness
				for(unsigned i = 0; i < 12; i++)
				{
					for(unsigned j = 0; j < 12; j++)
					{
						k[i + 12 * j] = 0;
						for(unsigned p = 0; p < 6; p++)
						{
							for(unsigned q = 0; q < 6; q++)
							{
								k[i + 12 * j] += T[p + 6 * i] * T[q + 6 * j] * m_k[p + 6 * q];
							}
						}
					}
				}
				//geometric stiffness
				if(m_geometric)
				{
					const double nljx = m_f[0];
					const double mliz = m_f[1];
					const double mljz = m_f[2];
					const double mljx = m_f[3];
					const double mliy = m_f[4];
					const double mljy = m_f[5];
					const double nljy = -(mliz + mljz) / l;
					const double nljz = +(mliy + mljy) / l;
					const mat::vec3 bi = mat::cross(t2, ri3) / mat::dot(t3, ri3 + rj3);
					const mat::vec3 bj = mat::cross(t2, rj3) / mat::dot(t3, ri3 + rj3);
					const mat::vec3 ar = mat::dot(t1, ri3 + rj3) / mat::dot(t3, ri3 + rj3) * t2;
					for(unsigned i = 0; i < 3; i++)
					{
						for(unsigned j = 0; j < 3; j++)
						{
							const double b2i = t2[i] * bi[j];
							const double b2j = t2[i] * bj[j];
							const double b3i = t3[i] * bi[j];
							const double b3j = t3[i] * bj[j];
							const double P11 = t2[i] * t2[j] + t3[i] * t3[j];
							const double H12 = t1[i] * t2[j] - t3[i] * ar[j];
							const double H13 = t1[i] * t3[j] + t2[i] * ar[j];
							const double Q12 = t1[i] * t2[j] + t2[i] * t1[j] - t3[i] * ar[j];
							const double Q13 = t1[i] * t3[j] + t3[i] * t1[j] + t2[i] * ar[j];
							k[i + 0 + 12 * (j + 3)] += nljz * b2i - nljy * b3i;
							k[i + 6 + 12 * (j + 3)] -= nljz * b2i - nljy * b3i;
							k[i + 0 + 12 * (j + 9)] += nljz * b2j - nljy * b3j;
							k[i + 6 + 12 * (j + 9)] -= nljz * b2j - nljy * b3j;
							k[i + 3 + 12 * (j + 3)] += mliy * b3i - mliz * b2i;
							k[i + 3 + 12 * (j + 9)] += mliy * b3j - mliz * b2j;
							k[i + 9 + 12 * (j + 3)] += mljy * b3i - mljz * b2i;
							k[i + 9 + 12 * (j + 9)] += mljy * b3j - mljz * b2j;
							k[i + 0 + 12 * (j + 0)] += (nljx * P11 - nljy * Q12 - nljz * Q13) / l;
							k[i + 0 + 12 * (j + 6)] -= (nljx * P11 - nljy * Q12 - nljz * Q13) / l;
							k[i + 6 + 12 * (j + 0)] -= (nljx * P11 - nljy * Q12 - nljz * Q13) / l;
							k[i + 6 + 12 * (j + 6)] += (nljx * P11 - nljy * Q12 - nljz * Q13) / l;
							k[i + 3 + 12 * (j + 0)] += (mliy * H12 + mliz * H13 + mljx * P11) / l;
							k[i + 3 + 12 * (j + 6)] -= (mliy * H12 + mliz * H13 + mljx * P11) / l;
							k[i + 9 + 12 * (j + 0)] += (mljy * H12 + mljz * H13 - mljx * P11) / l;
							k[i + 9 + 12 * (j + 6)] -= (mljy * H12 + mljz * H13 - mljx * P11) / l;
						}
					}
				}
				//return
				return k;
			}
			
			//static attributes
			bool Beam3::m_shear = false;
			bool Beam3::m_mixed = false;
		}
	}
}
