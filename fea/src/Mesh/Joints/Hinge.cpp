//std
#include <cmath>
#include <cfloat>
#include <cstring>
#include <GL/gl.h>

//mat
#include "misc/util.h"
#include "linear/quat.h"
#include "linear/vec3.h"
#include "linear/mat3.h"
#include "linear/lin3.h"

//fea
#include "Mesh/Mesh.h"
#include "Mesh/Nodes/Dofs.h"
#include "Mesh/Nodes/Node.h"
#include "Mesh/Joints/Types.h"
#include "Mesh/Joints/Hinge.h"
#include "Mesh/Joints/States.h"

#include "Boundary/Dependencies/Dependency.h"

namespace fea
{
	namespace mesh
	{
		namespace joints
		{
			//constructors
			Hinge::Hinge(void) : 
			m_moment_yield(DBL_MAX), m_plastic_modulus(0), 
			m_limit_rotation(M_PI / 18), m_limit_translation(1e-2),
			m_hardening_old(0), m_hardening_new(0), m_plastic_rotation_old(0), m_plastic_rotation_new(0)
			{
				m_axis[0] = 0;
				m_axis[1] = 0;
				m_axis[2] = 1;
				m_orientation[0] = 1;
				m_orientation[1] = 0;
				m_orientation[2] = 0;
				m_stiffness[0][2] = 0;
				m_stiffness[0][0] = 1.0e5;
				m_stiffness[0][1] = 1.0e5;
				m_stiffness[1][0] = 1.0e10;
				m_stiffness[1][1] = 1.0e10;
				m_stiffness[1][2] = 1.0e10; 
				memset(m_f, 0,  6 * sizeof(double));
				memset(m_k, 0, 18 * sizeof(double));
			}

			//destructor
			Hinge::~Hinge(void)
			{
				return;
			}

			//serialization
			void Hinge::load(FILE* file)
			{
				//base call
				Joint::load(file);
				//load axis
				fscanf(file, "%lf %lf %lf", &m_axis[0], &m_axis[1], &m_axis[2]);
				//load parameters
				fscanf(file, "%lf %lf %lf", &m_moment_yield, &m_limit_rotation, &m_plastic_modulus);
				//load orientation
				fscanf(file, "%lf %lf %lf", &m_orientation[0], &m_orientation[1], &m_orientation[2]);
				//load stiffness rotation
				fscanf(file, "%lf %lf %lf", &m_stiffness[0][0], &m_stiffness[0][1], &m_stiffness[0][2]);
				//load stiffness translation
				fscanf(file, "%lf %lf %lf", &m_stiffness[1][0], &m_stiffness[1][1], &m_stiffness[1][2]);
			}
			void Hinge::save(FILE* file) const
			{
				//base call
				Joint::save(file);
				//save axis
				fprintf(file, "%+.6e %+.6e %+.6e ", m_axis[0], m_axis[1], m_axis[2]);
				//save parameters
				fprintf(file, "%+.6e %+.6e %+.6e ", m_moment_yield, m_limit_rotation, m_plastic_modulus);
				//save orientation
				fprintf(file, "%+.6e %+.6e %+.6e ", m_orientation[0], m_orientation[1], m_orientation[2]);
				//save stiffness rotation
				fprintf(file, "%+.6e %+.6e %+.6e ", m_stiffness[0][0], m_stiffness[0][1], m_stiffness[0][2]);
				//save stiffness translation
				fprintf(file, "%+.6e %+.6e %+.6e ", m_stiffness[1][0], m_stiffness[1][1], m_stiffness[1][2]);
			}

			//types
			unsigned Hinge::states(void) const
			{
				return 
					(unsigned) joints::state::axial |
					(unsigned) joints::state::shear_y |
					(unsigned) joints::state::shear_z |
					(unsigned) joints::state::torsion |
					(unsigned) joints::state::bending_y |
					(unsigned) joints::state::bending_z;
			}
			joints::type Hinge::type(void) const
			{
				return joints::type::hinge;
			}

			//data
			const double* Hinge::axis(void) const
			{
				return m_axis;
			}
			const double* Hinge::axis(const double* axis)
			{
				return (const double*) memcpy(m_axis, axis, 3 * sizeof(double));
			}
			const double* Hinge::axis(double ax, double ay, double az)
			{
				m_axis[0] = ax;
				m_axis[1] = ay;
				m_axis[2] = az;
				return m_axis;
			}
			const double* Hinge::orientation(void) const
			{
				return m_orientation;
			}
			const double* Hinge::orientation(const double* orientation)
			{
				return (const double*) memcpy(m_orientation, orientation, 3 * sizeof(double));
			}
			const double* Hinge::orientation(double sx, double sy, double sz)
			{
				m_orientation[0] = sx;
				m_orientation[1] = sy;
				m_orientation[2] = sz;
				return m_orientation;
			}
			
			const double* Hinge::stiffness(unsigned i) const
			{
				return m_stiffness[i];
			}
			const double* Hinge::stiffness(unsigned i, const double* stiffness)
			{
				return (const double*) memcpy(m_stiffness[i], stiffness, 3 * sizeof(double));
			}
			const double* Hinge::stiffness(unsigned i, unsigned j, double k)
			{
				m_stiffness[i][j] = k;
				return m_stiffness[i];
			}
			const double* Hinge::stiffness(unsigned i, double k1, double k2, double k3)
			{
				m_stiffness[i][0] = k1;
				m_stiffness[i][1] = k2;
				m_stiffness[i][2] = k3;
				return m_stiffness[i];
			}
			
			double Hinge::moment_yield(void) const
			{
				return m_moment_yield;
			}
			double Hinge::moment_yield(double moment_yield)
			{
				return m_moment_yield = moment_yield;
			}
			double Hinge::plastic_modulus(void) const
			{
				return m_plastic_modulus;
			}
			double Hinge::plastic_modulus(double plastic_modulus)
			{
				return m_plastic_modulus = plastic_modulus;
			}

			double Hinge::limit_rotation(void) const
			{
				return m_limit_rotation;
			}
			double Hinge::limit_rotation(double limit_rotation)
			{
				return m_limit_rotation = limit_rotation;
			}
			double Hinge::limit_translation(void) const
			{
				return m_limit_translation;
			}
			double Hinge::limit_translation(double limit_translation)
			{
				return m_limit_translation = limit_translation;
			}
			
			//dofs
			std::vector<unsigned> Hinge::dofs(void) const
			{
				const unsigned dof = 
					(unsigned) nodes::dof::rotation_x |
					(unsigned) nodes::dof::rotation_y |
					(unsigned) nodes::dof::rotation_z |
					(unsigned) nodes::dof::translation_x |
					(unsigned) nodes::dof::translation_y |
					(unsigned) nodes::dof::translation_z;
				return {dof, dof};
			}

			//analysis
			void Hinge::apply(void)
			{
				//initial triad
				const mat::vec3 s3 = m_axis;
				const mat::vec3 s1 = m_orientation;
				const mat::vec3 s2 = mat::cross(s3, s1);
				//nodal positions
				double pi[3], pj[3];
				const mat::vec3 xi = node(0)->position(pi);
				const mat::vec3 xj = node(1)->position(pj);
				const mat::vec3 Xi = node(0)->coordinates();
				const mat::vec3 Xj = node(1)->coordinates();
				//nodal quaternions
				const mat::quat qi = node(0)->quaternion();
				const mat::quat qj = node(1)->quaternion();
				//nodal triads
				const mat::vec3 u1 = qi.rotate(s1);
				const mat::vec3 u2 = qi.rotate(s2);
				const mat::vec3 u3 = qi.rotate(s3);
				const mat::vec3 e1 = qj.rotate(s1);
				const mat::vec3 e2 = qj.rotate(s2);
				const mat::vec3 e3 = qj.rotate(s3);
				//rotation norm
				const double c1 = mat::dot(e1, u1);
				const double c2 = mat::dot(e2, u2);
				const double c3 = mat::dot(e3, u3);
				const double tn = acos(mat::bound((c1 + c2 + c3 - 1) / 2));
				//translation components
				const double d1 = mat::dot(u1, xj - xi) - mat::dot(s1, Xj - Xi);
				const double d2 = mat::dot(u2, xj - xi) - mat::dot(s2, Xj - Xi);
				const double d3 = mat::dot(u3, xj - xi) - mat::dot(s3, Xj - Xi);
				//rotation components
				const double tr = 1e-6;
				const double sn = sin(tn);
				const double fn = fabs(tn) < tr ? 1 : tn / sn;
				const double t1 = fn * (mat::dot(e2, u3) - mat::dot(e3, u2)) / 2;
				const double t2 = fn * (mat::dot(e3, u1) - mat::dot(e1, u3)) / 2;
				const double t3 = fn * (mat::dot(e1, u2) - mat::dot(e2, u1)) / 2;
				//rotation stiffness
				const double k1 = m_stiffness[0][0];
				const double k2 = m_stiffness[0][1];
				const double k3 = m_stiffness[0][2];
				//translation stiffness
				const double K1 = m_stiffness[1][0];
				const double K2 = m_stiffness[1][1];
				const double K3 = m_stiffness[1][2];
				//plasticity
				const double my = m_moment_yield;
				const double kp = m_plastic_modulus;
				//limits
				const double tl = m_limit_rotation;
				const double dl = m_limit_translation;
				//predictor
				const double a0 = m_hardening_old;
				const double t0 = m_plastic_rotation_old;
				const double f0 = k3 * fabs(t3 - t0) - (my + kp * a0);
				//return mapping
				double& ap = m_hardening_new = a0;
				double& tp = m_plastic_rotation_new = t0;
				if(f0 > 0)
				{
					ap += f0 / (k3 + kp);
					tp += f0 / (k3 + kp) * mat::sign(t3 - t0);
				}
				//moments
				m_f[2] = K3 * d3;
				m_f[5] = k3 * (t3 - tp);
				m_f[0] = K1 * log((dl + d1) / (dl - d1)) * dl / 2;
				m_f[1] = K2 * log((dl + d2) / (dl - d2)) * dl / 2;
				m_f[3] = k1 * log((tl + t1) / (tl - t1)) * tl / 2;
				m_f[4] = k2 * log((tl + t2) / (tl - t2)) * tl / 2;
				//stiffness
				m_k[ 8] = K3;
				m_k[17] = f0 < 0 ? k3 : k3 * kp / (k3 + kp);
				m_k[ 0] = K1 * dl * dl / (dl * dl - d1 * d1);
				m_k[ 4] = K2 * dl * dl / (dl * dl - d2 * d2);
				m_k[ 9] = k1 * tl * tl / (tl * tl - t1 * t1);
				m_k[13] = k2 * tl * tl / (tl * tl - t2 * t2);
			}
			
			void Hinge::prepare(void)
			{
				//base call
				Joint::prepare();
				//orientation
				mat::vec3(m_orientation).project(m_axis);
			}
			
			void Hinge::record(void)
			{
				//states
				const double nix = -m_f[2];
				const double njx = +m_f[2];
				const double niy = -m_f[0];
				const double njy = +m_f[0];
				const double niz = -m_f[1];
				const double njz = +m_f[1];
				const double mix = -m_f[5];
				const double mjx = +m_f[5];
				const double miy = -m_f[3];
				const double mjy = +m_f[3];
				const double miz = -m_f[4];
				const double mjz = +m_f[4];
				//record
				char formatter[200];
				const double s[2][6] = {
					{nix, niy, niz, mix, miy, miz}, 
					{njx, njy, njz, mjx, mjy, mjz}
				};
				for(unsigned i = 0; i < 2; i++)
				{
					for(unsigned j = 0; j < 6; j++)
					{
						sprintf(formatter, "%+.6e ", s[i][j]);
						m_results += formatter;
					}
				}
				m_results += "\n";
			}
			void Hinge::update(void)
			{
				m_hardening_old = m_hardening_new;
				m_plastic_rotation_old = m_plastic_rotation_new;
			}
			void Hinge::restore(void)
			{
				m_hardening_new = m_hardening_old;
				m_plastic_rotation_new = m_plastic_rotation_old;
			}
			
			//formulation
			double* Hinge::internal_force(double* f) const
			{
				//initial triad
				const mat::vec3 s3 = m_axis;
				const mat::vec3 s1 = m_orientation;
				const mat::vec3 s2 = mat::cross(s3, s1);
				//nodal position
				double pi[3], pj[3];
				const mat::vec3 xi = node(0)->position(pi);
				const mat::vec3 xj = node(1)->position(pj);
				//nodal quaternions
				const mat::quat qi = node(0)->quaternion();
				const mat::quat qj = node(1)->quaternion();
				//nodal triads
				const mat::vec3 u1 = qi.rotate(s1);
				const mat::vec3 u2 = qi.rotate(s2);
				const mat::vec3 u3 = qi.rotate(s3);
				const mat::vec3 e1 = qj.rotate(s1);
				const mat::vec3 e2 = qj.rotate(s2);
				const mat::vec3 e3 = qj.rotate(s3);
				//rotation norm
				const double c1 = mat::dot(e1, u1);
				const double c2 = mat::dot(e2, u2);
				const double c3 = mat::dot(e3, u3);
				const double tn = acos(mat::bound((c1 + c2 + c3 - 1) / 2));
				//rotation components
				const double tr = 1e-6;
				const double cn = cos(tn);
				const double sn = sin(tn);
				const double fn = fabs(tn) < tr ? 1 : tn / sn;
				const double gn = fabs(tn) < tr ? 0 : 1 / tn - cn / sn;
				const double t1 = fn * (mat::dot(e2, u3) - mat::dot(e3, u2)) / 2;
				const double t2 = fn * (mat::dot(e3, u1) - mat::dot(e1, u3)) / 2;
				const double t3 = fn * (mat::dot(e1, u2) - mat::dot(e2, u1)) / 2;
				//rotation direction
				const mat::vec3 nr = fabs(tn) < tr ? s3 : (t1 * s1 + t2 * s2 + t3 * s3) / tn;
				//translation gradient
				const mat::vec3 w1 = mat::cross(u1, xj - xi);
				const mat::vec3 w2 = mat::cross(u2, xj - xi);
				const mat::vec3 w3 = mat::cross(u3, xj - xi);
				//rotation gradient
				const mat::vec3 q1 = gn * t1 * nr + fn * (mat::cross(e2, u3) - mat::cross(e3, u2)) / 2;
				const mat::vec3 q2 = gn * t2 * nr + fn * (mat::cross(e3, u1) - mat::cross(e1, u3)) / 2;
				const mat::vec3 q3 = gn * t3 * nr + fn * (mat::cross(e1, u2) - mat::cross(e2, u1)) / 2;
				//internal force
				memset(f, 0, 12 * sizeof(double));
				const mat::vec3 uv[] = {u1, u2, u3};
				const mat::vec3 wv[] = {w1, w2, w3};
				const mat::vec3 qv[] = {q1, q2, q3};
				for(unsigned i = 0; i < 3; i++)
				{
					for(unsigned j = 0; j < 3; j++)
					{
						f[j + 0] -= m_f[i + 0] * uv[i][j];
						f[j + 6] += m_f[i + 0] * uv[i][j];
						f[j + 3] -= m_f[i + 3] * qv[i][j];
						f[j + 9] += m_f[i + 3] * qv[i][j];
						f[j + 3] += m_f[i + 0] * wv[i][j];
					}
				}
				//return
				return f;
			}
			
			double* Hinge::damping(double* c) const
			{
				return c;
			}
			double* Hinge::stiffness(double* k) const
			{
				//initial triad
				const mat::vec3 s3 = m_axis;
				const mat::vec3 s1 = m_orientation;
				const mat::vec3 s2 = mat::cross(s3, s1);
				//nodal position
				double pi[3], pj[3];
				const mat::vec3 xi = node(0)->position(pi);
				const mat::vec3 xj = node(1)->position(pj);
				//nodal quaternions
				const mat::quat qi = node(0)->quaternion();
				const mat::quat qj = node(1)->quaternion();
				//nodal triads
				const mat::vec3 u1 = qi.rotate(s1);
				const mat::vec3 u2 = qi.rotate(s2);
				const mat::vec3 u3 = qi.rotate(s3);
				const mat::vec3 e1 = qj.rotate(s1);
				const mat::vec3 e2 = qj.rotate(s2);
				const mat::vec3 e3 = qj.rotate(s3);
				//rotation norm
				const double c1 = mat::dot(e1, u1);
				const double c2 = mat::dot(e2, u2);
				const double c3 = mat::dot(e3, u3);
				const double tn = acos(mat::bound((c1 + c2 + c3 - 1) / 2));
				//components
				const double tr = 1e-6;
				const double cn = cos(tn);
				const double sn = sin(tn);
				const double fn = fabs(tn) < tr ? 1 : tn / sn;
				const double gn = fabs(tn) < tr ? 0 : 1 / tn - cn / sn;
				const double t1 = fn * (mat::dot(e2, u3) - mat::dot(e3, u2)) / 2;
				const double t2 = fn * (mat::dot(e3, u1) - mat::dot(e1, u3)) / 2;
				const double t3 = fn * (mat::dot(e1, u2) - mat::dot(e2, u1)) / 2;
				//rotation direction
				const mat::vec3 nr = fabs(tn) < tr ? s3 : (t1 * s1 + t2 * s2 + t3 * s3) / tn;
				//translation gradient
				const mat::vec3 xr = xj - xi;
				const mat::vec3 w1 = mat::cross(u1, xr);
				const mat::vec3 w2 = mat::cross(u2, xr);
				const mat::vec3 w3 = mat::cross(u3, xr);
				//rotation gradient
				const mat::vec3 q1 = gn * t1 * nr + fn * (mat::cross(e2, u3) - mat::cross(e3, u2)) / 2;
				const mat::vec3 q2 = gn * t2 * nr + fn * (mat::cross(e3, u1) - mat::cross(e1, u3)) / 2;
				const mat::vec3 q3 = gn * t3 * nr + fn * (mat::cross(e1, u2) - mat::cross(e2, u1)) / 2;
				//rotation spins
				const mat::mat3 su1 = u1.spin();
				const mat::mat3 su2 = u2.spin();
				const mat::mat3 su3 = u3.spin();
				const mat::mat3 se1 = e1.spin();
				const mat::mat3 se2 = e2.spin();
				const mat::mat3 se3 = e3.spin();
				//translation spins
				const mat::mat3 sxr = xr.spin();
				const mat::mat3 sd1 = sxr * su1;
				const mat::mat3 sd2 = sxr * su2;
				const mat::mat3 sd3 = sxr * su3;
				//rotation hessian
				const mat::mat3 Qi1 = fn * (se2 * su3 - se3 * su2) / 2;
				const mat::mat3 Qi2 = fn * (se3 * su1 - se1 * su3) / 2;
				const mat::mat3 Qi3 = fn * (se1 * su2 - se2 * su1) / 2;
				const mat::mat3 Qj1 = fn * (su3 * se2 - su2 * se3) / 2;
				const mat::mat3 Qj2 = fn * (su1 * se3 - su3 * se1) / 2;
				const mat::mat3 Qj3 = fn * (su2 * se1 - su1 * se2) / 2;
				//stiffness
				memset(k, 0, 144 * sizeof(double));
				const mat::vec3 uv[] = {u1, u2, u3};
				const mat::vec3 wv[] = {w1, w2, w3};
				const mat::vec3 qv[] = {q1, q2, q3};
				const mat::mat3 suv[] = {su1, su2, su3};
				const mat::mat3 sdv[] = {sd1, sd2, sd3};
				const mat::mat3 Qiv[] = {Qi1, Qi2, Qi3};
				const mat::mat3 Qjv[] = {Qj1, Qj2, Qj3};
				for(unsigned a = 0; a < 3; a++)
				{
					for(unsigned b = 0; b < 3; b++)
					{
						for(unsigned i = 0; i < 3; i++)
						{
							for(unsigned j = 0; j < 3; j++)
							{
								k[a + 0 + 12 * (b + 0)] += m_k[i + 0 + 3 * j] * uv[i][a] * uv[j][b];
								k[a + 0 + 12 * (b + 6)] -= m_k[i + 0 + 3 * j] * uv[i][a] * uv[j][b];
								k[a + 6 + 12 * (b + 0)] -= m_k[i + 0 + 3 * j] * uv[i][a] * uv[j][b];
								k[a + 6 + 12 * (b + 6)] += m_k[i + 0 + 3 * j] * uv[i][a] * uv[j][b];
								k[a + 0 + 12 * (b + 3)] -= m_k[i + 0 + 3 * j] * uv[i][a] * wv[j][b];
								k[a + 6 + 12 * (b + 3)] += m_k[i + 0 + 3 * j] * uv[i][a] * wv[j][b];
								k[a + 3 + 12 * (b + 0)] -= m_k[i + 0 + 3 * j] * wv[i][a] * uv[j][b];
								k[a + 3 + 12 * (b + 6)] += m_k[i + 0 + 3 * j] * wv[i][a] * uv[j][b];
								k[a + 3 + 12 * (b + 9)] -= m_k[i + 9 + 3 * j] * qv[i][a] * qv[j][b];
								k[a + 9 + 12 * (b + 3)] -= m_k[i + 9 + 3 * j] * qv[i][a] * qv[j][b];
								k[a + 9 + 12 * (b + 9)] += m_k[i + 9 + 3 * j] * qv[i][a] * qv[j][b];
								k[a + 3 + 12 * (b + 3)] += m_k[i + 9 + 3 * j] * qv[i][a] * qv[j][b];
								k[a + 3 + 12 * (b + 3)] += m_k[i + 0 + 3 * j] * wv[i][a] * wv[j][b];
							}
							k[a + 0 + 12 * (b + 3)] += m_f[i + 0] * suv[i][a + 3 * b];
							k[a + 6 + 12 * (b + 3)] -= m_f[i + 0] * suv[i][a + 3 * b];
							k[a + 3 + 12 * (b + 0)] -= m_f[i + 0] * suv[i][a + 3 * b];
							k[a + 3 + 12 * (b + 6)] += m_f[i + 0] * suv[i][a + 3 * b];
							k[a + 3 + 12 * (b + 9)] -= m_f[i + 3] * Qjv[i][a + 3 * b];
							k[a + 9 + 12 * (b + 3)] -= m_f[i + 3] * Qiv[i][a + 3 * b];
							k[a + 9 + 12 * (b + 9)] += m_f[i + 3] * Qjv[i][a + 3 * b];
							k[a + 3 + 12 * (b + 3)] += m_f[i + 3] * Qiv[i][a + 3 * b];
							k[a + 3 + 12 * (b + 3)] += m_f[i + 0] * sdv[i][a + 3 * b];
						}
					}
				}
				//return
				return k;
			}
			
			//results
			void Hinge::plot(double, const double* color, const double** positions) const
			{
				//configuration
				const double* pi = positions ? positions[m_nodes[0]] : node(0)->coordinates();
				const double* pj = positions ? positions[m_nodes[1]] : node(1)->coordinates();
				//plot
				glLineWidth(2);
				glColor4dv(color);
				glBegin(GL_LINE_STRIP);
					glVertex3dv(pi);
					glVertex3dv(pj);
				glEnd();
			}
		}
	}
}

