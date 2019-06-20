//std
#include <cmath>
#include <cstring>
#include <GL/gl.h>

//mat
#include "misc/defs.h"

//fea
#include "Mesh/Mesh.h"
#include "Mesh/Nodes/Dofs.h"
#include "Mesh/Nodes/Node.h"
#include "Mesh/Joints/Types.h"
#include "Mesh/Joints/States.h"
#include "Mesh/Joints/Spherical.h"

#include "Boundary/Dependencies/Dependency.h"

namespace fea
{
	namespace mesh
	{
		namespace joints
		{
			//constructors
			Spherical::Spherical(void) : m_damping(0), m_stiffness(0)
			{
				return;
			}

			//destructor
			Spherical::~Spherical(void)
			{
				return;
			}
			
			//serialization
			void Spherical::load(FILE* file)
			{
				//base call
				Joint::load(file);
				//load stiffness
				fscanf(file, "%lf %lf", &m_damping, &m_stiffness);
			}
			void Spherical::save(FILE* file) const
			{
				//base call
				Joint::save(file);
				//save stiffness
				fprintf(file, "%+.6e %+.6e ", m_damping, m_stiffness);
			}

			//data
			double Spherical::damping(void) const
			{
				return m_damping;
			}
			double Spherical::damping(double damping)
			{
				return m_damping = damping;
			}
			double Spherical::stiffness(void) const
			{
				return m_stiffness;
			}
			double Spherical::stiffness(double stiffness)
			{
				return m_stiffness = stiffness;
			}

			//types
			unsigned Spherical::states(void) const
			{
				return 0;
			}
			joints::type Spherical::type(void) const
			{
				return joints::type::spherical;
			}

			//dofs
			std::vector<unsigned> Spherical::dofs(void) const
			{
				const unsigned dof = 
					(unsigned) nodes::dof::translation_x |
					(unsigned) nodes::dof::translation_y |
					(unsigned) nodes::dof::translation_z;
				return {dof, dof};
			}

			//analysis
			void Spherical::apply(void)
			{
				return;
			}

			//formulation
			double* Spherical::internal_force(double* f) const
			{
				//position
				double xi[3], xj[3];
				node(0)->position(xi);
				node(1)->position(xj);
				//stiffness
				const double k = m_stiffness;
				//internal force
				f[0] = -k * (xj[0] - xi[0]);
				f[1] = -k * (xj[1] - xi[1]);
				f[2] = -k * (xj[2] - xi[2]);
				f[3] = +k * (xj[0] - xi[0]);
				f[4] = +k * (xj[1] - xi[1]);
				f[5] = +k * (xj[2] - xi[2]);
				//return
				return f;
			}
			
			double* Spherical::damping(double* c) const
			{
				return c;
			}
			double* Spherical::stiffness(double* k) const
			{
				//stiffness
				const double kh = m_stiffness;
				memset(k, 0, 36 * sizeof(double));
				k[0 + 6 * 0] = +kh;
				k[1 + 6 * 1] = +kh;
				k[2 + 6 * 2] = +kh;
				k[3 + 6 * 3] = +kh;
				k[4 + 6 * 4] = +kh;
				k[5 + 6 * 5] = +kh;
				k[0 + 6 * 3] = -kh;
				k[1 + 6 * 4] = -kh;
				k[2 + 6 * 5] = -kh;
				k[3 + 6 * 0] = -kh;
				k[4 + 6 * 1] = -kh;
				k[5 + 6 * 2] = -kh;
				return k;
			}
			
			//results
			void Spherical::plot(double size, const double* color, const double** positions) const
			{
				//mesh
				const unsigned n = 10;
				//position and size
				const double s = size / 2;
				const double* p = positions ? positions[m_nodes[0]] : node(0)->coordinates();
				//setup
				glLineWidth(2);
				glColor4dv(color);
				//plot x
				for(unsigned i = 0; i < n + 1; i++)
				{
					glBegin(GL_LINE_STRIP);
					const double a = 2 * i * M_PI / n;
					for(unsigned j = 0; j < n + 1; j++)
					{
						const double b = 2 * j * M_PI / n;
						glVertex3d(p[0] + s * sin(a), p[1] + s * cos(a) * cos(b), p[2] + s * cos(a) * sin(b));
					}
					glEnd();
				}
				//plot y
				for(unsigned i = 0; i < n + 1; i++)
				{
					glBegin(GL_LINE_STRIP);
					const double a = 2 * i * M_PI / n;
					for(unsigned j = 0; j < n + 1; j++)
					{
						const double b = 2 * j * M_PI / n;
						glVertex3d(p[0] + s * cos(a) * cos(b), p[1] + s * sin(a), p[2] + s * cos(a) * sin(b));
					}
					glEnd();
				}
				//plot z
				for(unsigned i = 0; i < n + 1; i++)
				{
					glBegin(GL_LINE_STRIP);
					const double a = 2 * i * M_PI / n;
					for(unsigned j = 0; j < n + 1; j++)
					{
						const double b = 2 * j * M_PI / n;
						glVertex3d(p[0] + s * cos(a) * cos(b), p[1] + s * cos(a) * sin(b), p[2] + s * sin(a));
					}
					glEnd();
				}
			}
		}
	}
}
