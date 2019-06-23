//std
#include <cmath>
#include <cfloat>
#include <cstring>
#include <GL/gl.h>

//mat
#include "misc/defs.h"
#include "misc/util.h"
#include "linear/dense.h"

//fea
#include "Mesh/Mesh.h"
#include "Mesh/Nodes/Dofs.h"
#include "Mesh/Nodes/Node.h"
#include "Mesh/Joints/Types.h"
#include "Mesh/Joints/States.h"
#include "Mesh/Joints/Pinned.h"

#include "Boundary/Dependencies/Dependency.h"

namespace fea
{
	namespace mesh
	{
		namespace joints
		{
			//constructor
			Pinned::Pinned(void) :  
				m_fixed(false), m_clearance(1e-3), m_orientation(0), m_moment_yield(DBL_MAX),
				m_hardening_old(0), m_hardening_new(0), m_plastic_modulus(0), m_plastic_rotation_old(0), m_plastic_rotation_new(0)
			{
				m_stiffness[2] = 0;
				m_stiffness[0] = 1e10;
				m_stiffness[1] = 1e10;
				memset(m_f, 0, 3 * sizeof(double));
				memset(m_k, 0, 5 * sizeof(double));
			}

			//destructor
			Pinned::~Pinned(void)
			{
				return;
			}
			
			//serialization
			void Pinned::load(FILE* file)
			{
				//base call
				Joint::load(file);
				//load fixed
				fscanf(file, "%d", &m_fixed);
				//load clearance
				fscanf(file, "%lf", &m_clearance);
				//load orientation
				fscanf(file, "%lf", &m_orientation);
				//load parameters
				fscanf(file, "%lf %lf", &m_moment_yield, &m_plastic_modulus);
				//load stiffness
				fscanf(file, "%lf %lf %lf", &m_stiffness[0], &m_stiffness[1], &m_stiffness[2]);
			}
			void Pinned::save(FILE* file) const
			{
				//base call
				Joint::save(file);
				//save fixed
				fprintf(file, "%02d", m_fixed);
				//save clearance
				fprintf(file, "%+.6e ", m_clearance);
				//save orientation
				fprintf(file, "%+.6e ", m_orientation);
				//save parameters
				fprintf(file, "%+.6e %+.6e ", m_moment_yield, m_plastic_modulus);
				//save stiffness
				fprintf(file, "%+.6e %+.6e %+.6e ", m_stiffness[0], m_stiffness[1], m_stiffness[2]);
			}

			//data
			bool Pinned::fixed(void) const
			{
				return m_fixed;
			}
			bool Pinned::fixed(bool fixed)
			{
				return m_fixed = fixed;
			}
			
			double Pinned::clearance(void) const
			{
				return m_clearance;
			}
			double Pinned::clearance(double clearance)
			{
				return m_clearance = clearance;
			}
			double Pinned::orientation(void) const
			{
				return m_orientation;
			}
			double Pinned::orientation(double orientation)
			{
				return m_orientation = orientation;
			}
			double Pinned::moment_yield(void) const
			{
				return m_moment_yield;
			}
			double Pinned::moment_yield(double moment_yield)
			{
				return m_moment_yield = moment_yield;
			}
			double Pinned::plastic_modulus(void) const
			{
				return m_plastic_modulus;
			}
			double Pinned::plastic_modulus(double plastic_modulus)
			{
				return m_plastic_modulus = plastic_modulus;
			}
			const double* Pinned::stiffness(void) const
			{
				return m_stiffness;
			}
			const double* Pinned::stiffness(double kt)
			{
				m_stiffness[2] = kt;
				return m_stiffness;
			}
			const double* Pinned::stiffness(const double* stiffness)
			{
				return (const double*) memcpy(m_stiffness, stiffness, 3 * sizeof(double));
			}
			const double* Pinned::stiffness(double k1, double k2, double kt)
			{
				m_stiffness[0] = k1;
				m_stiffness[1] = k2;
				m_stiffness[2] = kt;
				return m_stiffness;
			}

			//types
			unsigned Pinned::states(void) const
			{
				return m_fixed ? 
					(unsigned) joints::state::torsion :
					(unsigned) joints::state::shear_y |
					(unsigned) joints::state::shear_z |
					(unsigned) joints::state::torsion;
			}
			joints::type Pinned::type(void) const
			{
				return joints::type::pinned;
			}

			//dofs
			std::vector<unsigned> Pinned::dofs(void) const
			{
				const unsigned dof = m_fixed ? 
					(unsigned) fea::mesh::nodes::dof::rotation_z :
					(unsigned) fea::mesh::nodes::dof::rotation_z |
					(unsigned) fea::mesh::nodes::dof::translation_x |
					(unsigned) fea::mesh::nodes::dof::translation_y;
				return {dof, dof};
			}

			//analysis
			void Pinned::apply(void)
			{
				//kinematics
				double pi[3], pj[3];
				const double* xi = node(0)->position(pi);
				const double* xj = node(1)->position(pj);
				const double* Xi = node(0)->coordinates();
				const double* Xj = node(1)->coordinates();
				//rotations
				const double ti = node(0)->state(nodes::dof::rotation_z);
				const double tj = node(1)->state(nodes::dof::rotation_z);
				//clearance
				const double ul = m_clearance;
				//orientation
				const double t0 = m_orientation;
				//directions
				const double c0 = cos(t0);
				const double s0 = sin(t0);
				const double ci = cos(t0 + ti);
				const double si = sin(t0 + ti);
				//stiffness
				const double k1 = m_stiffness[0];
				const double k2 = m_stiffness[1];
				const double kt = m_stiffness[2];
				//parameters
				const double my = m_moment_yield;
				const double kp = m_plastic_modulus;
				//displacements
				const double u1 = +ci * (xj[0] - xi[0]) + si * (xj[1] - xi[1]) - c0 * (Xj[0] - Xi[0]) - s0 * (Xj[1] - Xi[1]);
				const double u2 = -si * (xj[0] - xi[0]) + ci * (xj[1] - xi[1]) + s0 * (Xj[0] - Xi[0]) - c0 * (Xj[1] - Xi[1]);
				//predictor
				const double ar = m_hardening_old;
				const double tr = m_plastic_rotation_old;
				const double f0 = kt * fabs(tj - ti - tr) - (my + kp * ar);
				//return mapping
				double& ap = m_hardening_new = ar;
				double& tp = m_plastic_rotation_new = tr;
				if(f0 > 0)
				{
					ap += f0 / (kt + kp);
					tp += f0 / (kt + kp) * mat::sign(tj - ti - tr);
				}
				//moment
				m_f[2] = kt * (tj - ti - tp);
				m_f[0] = k1 * ul / 2 * log((ul + u1) / (ul - u1));
				m_f[1] = k2 * ul / 2 * log((ul + u2) / (ul - u2));
				//stiffness
				m_k[4] = f0 < 0 ? kt : kt * kp / (kt + kp);
				m_k[0] = k1 * ul * ul / (ul * ul - u1 * u1);
				m_k[3] = k2 * ul * ul / (ul * ul - u2 * u2);
			}
			
			bool Pinned::check(void) const
			{
				if(m_fixed)
				{
					const double s = m_mesh->size();
					const double* xi = node(0)->coordinates();
					const double* xj = node(1)->coordinates();
					const double d = mat::norm(xj, xi, 3);
					if(d > 1e-5 * s)
					{
						return false;
					}
				}
				return Joint::check();
			}
			void Pinned::configure(void)
			{
				if(m_fixed)
				{
					//create
					boundary::Dependency* dx = create();
					boundary::Dependency* dy = create();
					//set
					dx->slave_node(m_nodes[1]);
					dy->slave_node(m_nodes[1]);
					dx->master_node(0, m_nodes[0]);
					dy->master_node(0, m_nodes[0]);
					dx->slave_dof(nodes::dof::translation_x);
					dy->slave_dof(nodes::dof::translation_y);
					dx->master_dof(0, nodes::dof::translation_x);
					dy->master_dof(0, nodes::dof::translation_y);
				}
			}
			
			void Pinned::update(void)
			{
				m_hardening_old = m_hardening_new;
				m_plastic_rotation_old = m_plastic_rotation_new;
			}
			void Pinned::restore(void)
			{
				m_hardening_new = m_hardening_old;
				m_plastic_rotation_new = m_plastic_rotation_old;
			}
			
			void Pinned::record(void)
			{
				//kinematics
				double pi[3], pj[3];
				const double* xi = node(0)->position(pi);
				const double* xj = node(1)->position(pj);
				//orientation
				const double t0 = m_orientation;
				//rotation
				const double ti = node(0)->state(nodes::dof::rotation_z);
				//directions
				const double ci = cos(t0 + ti);
				const double si = sin(t0 + ti);
				//eccentricity
				const double x1 = +ci * (xj[0] - xi[0]) + si * (xj[1] - xi[1]);
				const double x2 = -si * (xj[0] - xi[0]) + ci * (xj[1] - xi[1]);
				//record
				char formatter[200];
				const double fxi = -m_f[0];
				const double fyi = -m_f[1];
				const double fxj = +m_f[0];
				const double fyj = +m_f[1];
				const double mzj = +m_f[2];
				const double mzi = -mzj + fxj * x2 - fyj * x1;
				const double s[2][3] = {{fxi, fyi, mzi}, {fxj, fyj, mzj}};
				for(unsigned i = 0; i < 2; i++)
				{
					for(unsigned j = 0; j < 3; j++)
					{
						if(!m_fixed || j == 2)
						{
							sprintf(formatter, "%+.6e ", s[i][j]);
							m_results += formatter;
						}
					}
				}
				m_results += "\n";
			}
			
			//formulation
			double* Pinned::internal_force(double* f) const
			{
				//kinematics
				double pi[3], pj[3];
				const double* xi = node(0)->position(pi);
				const double* xj = node(1)->position(pj);
				//orientation
				const double t0 = m_orientation;
				//rotation
				const double ti = node(0)->state(nodes::dof::rotation_z);
				//directions
				const double ci = cos(t0 + ti);
				const double si = sin(t0 + ti);
				//eccentricity
				const double x1 = +ci * (xj[0] - xi[0]) + si * (xj[1] - xi[1]);
				const double x2 = -si * (xj[0] - xi[0]) + ci * (xj[1] - xi[1]);
				//local force
				const double f1 = m_f[0];
				const double f2 = m_f[1];
				const double mt = m_f[2];
				//internal force
				if(m_fixed)
				{
					f[0] = -mt;
					f[1] = +mt;
				}
				else
				{
					f[5] = +mt;
					f[0] = -ci * f1 + si * f2;
					f[1] = -si * f1 - ci * f2;
					f[3] = +ci * f1 - si * f2;
					f[4] = +si * f1 + ci * f2;
					f[2] = -mt + f1 * x2 - f2 * x1;
				}
				//return
				return f;
			}
			
			double* Pinned::damping(double* c) const
			{
				return c;
			}
			double* Pinned::stiffness(double* k) const
			{
				//positions
				double pi[3], pj[3];
				const double* xi = node(0)->position(pi);
				const double* xj = node(1)->position(pj);
				//orientation
				const double t0 = m_orientation;
				//rotation
				const double ti = node(0)->state(nodes::dof::rotation_z);
				//directions
				const double ci = cos(t0 + ti);
				const double si = sin(t0 + ti);
				//displacements
				const double x1 = +ci * (xj[1] - xi[1]) + si * (xj[0] - xi[0]);
				const double x2 = -si * (xj[1] - xi[1]) + ci * (xj[0] - xi[0]);
				//local force
				const double f1 = m_f[0];
				const double f2 = m_f[1];
				//local stiffness
				const double k11 = m_k[0];
				const double k12 = m_k[2];
				const double k21 = m_k[1];
				const double k22 = m_k[3];
				const double ktt = m_k[4];
				//stiffness
				if(m_fixed)
				{
					k[2 * 0 + 1] = k[2 * 1 + 0] = -ktt;
					k[2 * 0 + 0] = k[2 * 1 + 1] = +ktt;
				}
				else
				{
					k[6 * 2 + 5] = -ktt;
					k[6 * 5 + 2] = -ktt;
					k[6 * 5 + 5] = +ktt;
					k[6 * 0 + 0] = +ci * ci * k11 + si * si * k22 - si * ci * (k12 + k21);
					k[6 * 0 + 1] = +ci * ci * k21 - si * si * k12 + si * ci * (k11 - k22);
					k[6 * 0 + 3] = -ci * ci * k11 - si * si * k22 + si * ci * (k12 + k21);
					k[6 * 0 + 4] = -ci * ci * k21 + si * si * k12 - si * ci * (k11 - k22);
					k[6 * 1 + 0] = +ci * ci * k12 - si * si * k21 + si * ci * (k11 - k22);
					k[6 * 1 + 1] = +si * si * k11 + ci * ci * k22 + si * ci * (k12 + k21);
					k[6 * 1 + 3] = -ci * ci * k12 + si * si * k21 - si * ci * (k11 - k22);
					k[6 * 1 + 4] = -si * si * k11 - ci * ci * k22 - si * ci * (k12 + k21);
					k[6 * 3 + 0] = -ci * ci * k11 - si * si * k22 + si * ci * (k12 + k21);
					k[6 * 3 + 1] = -ci * ci * k21 + si * si * k12 - si * ci * (k11 - k22);
					k[6 * 3 + 3] = +ci * ci * k11 + si * si * k22 - si * ci * (k12 + k21);
					k[6 * 3 + 4] = +ci * ci * k21 - si * si * k12 + si * ci * (k11 - k22);
					k[6 * 4 + 0] = -ci * ci * k12 + si * si * k21 - si * ci * (k11 - k22);
					k[6 * 4 + 1] = -si * si * k11 - ci * ci * k22 - si * ci * (k12 + k21);
					k[6 * 4 + 3] = +ci * ci * k12 - si * si * k21 + si * ci * (k11 - k22);
					k[6 * 4 + 4] = +si * si * k11 + ci * ci * k22 + si * ci * (k12 + k21);
					k[6 * 0 + 2] = +x2 * (si * k12 - ci * k11) - x1 * (si * k22 - ci * k21) + si * f1 + ci * f2;
					k[6 * 1 + 2] = -x2 * (si * k11 + ci * k12) + x1 * (ci * k22 + si * k21) - ci * f1 - si * f2;
					k[6 * 3 + 2] = -x2 * (si * k12 - ci * k11) + x1 * (si * k22 - ci * k21) - si * f1 - ci * f2;
					k[6 * 4 + 2] = +x2 * (si * k11 + ci * k12) - x1 * (ci * k22 + si * k21) + ci * f1 + si * f2;
					k[6 * 2 + 0] = -ci * (x2 * k11 - x1 * k12) + si * (x2 * k21 - x1 * k22) + si * f1 + ci * f2;
					k[6 * 2 + 1] = -si * (x2 * k11 - x1 * k12) - ci * (x2 * k21 - x1 * k22) - ci * f1 + si * f2;
					k[6 * 2 + 3] = +ci * (x2 * k11 - x1 * k12) - si * (x2 * k21 - x1 * k22) - si * f1 - ci * f2;
					k[6 * 2 + 4] = +si * (x2 * k11 - x1 * k12) + ci * (x2 * k21 - x1 * k22) + ci * f1 - si * f2;
					k[6 * 2 + 2] = +ktt + x2 * (x2 * k11 - x1 * k12) - x1 * (x2 * k21 - x1 * k22) - x1 * f1 - x2 * f2;
					k[6 * 0 + 5] = k[6 * 1 + 5] = k[6 * 3 + 5] = k[6 * 4 + 5] = k[6 * 5 + 0] = k[6 * 5 + 1] = k[6 * 5 + 3] = k[6 * 5 + 4] = 0;
				}
				//return
				return k;
			}
			
			//results
			void Pinned::plot(double size, const double* color, const double** positions) const
			{
				const unsigned nl = 5;
				const unsigned np = 80;
				const double* pi = positions ? positions[m_nodes[0]] : node(0)->coordinates();
				const double* pj = positions ? positions[m_nodes[1]] : node(1)->coordinates();
				glLineWidth(2);
				glColor4dv(color);
				glBegin(GL_LINE_STRIP);
				for(unsigned i = 0; i < np; i++)
				{
					const double r = 0.7 * i * size / (np - 1);
					const double t = 2 * nl * M_PI * i / (np - 1);
					glVertex3d(pi[0] + r * cos(t), pi[1] + r * sin(t), pi[2]);
				}
				glEnd();
				glBegin(GL_LINES);
				glVertex3dv(pi);
				glVertex3dv(pj);
				glEnd();
			}
		}
	}
}
