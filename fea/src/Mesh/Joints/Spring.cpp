//std
#include <cmath>
#include <cstring>

//fea
#include "Mesh/Nodes/Dofs.h"
#include "Mesh/Nodes/Node.h"

#include "Mesh/Joints/Types.h"
#include "Mesh/Joints/States.h"
#include "Mesh/Joints/Spring.h"

namespace fea
{
	namespace mesh
	{
		namespace joints
		{
			//constructors
			Spring::Spring(void) : m_stiffness(0), m_damping(0)
			{
				m_axis[0] = 0;
				m_axis[1] = 0;
				m_axis[2] = 1;
			}

			//constructors
			Spring::~Spring(void)
			{
				return;
			}

			//types
			unsigned Spring::states(void) const
			{
				return 0;
			}
			joints::type Spring::type(void) const
			{
				return joints::type::spring;
			}
			
			//dofs
			std::vector<unsigned> Spring::dofs(void) const
			{
				return {(unsigned) nodes::dof::translation_x | (unsigned) nodes::dof::translation_y | (unsigned) nodes::dof::translation_z};
			}

			//data
			double Spring::damping(void) const
			{
				return m_damping;
			}
			double Spring::damping(double damping)
			{
				return m_damping = damping;
			}
			double Spring::stiffness(void) const
			{
				return m_stiffness;
			}
			double Spring::stiffness(double stiffness)
			{
				return m_stiffness = stiffness;
			}
			const double* Spring::axis(void) const
			{
				return m_axis;
			}
			const double* Spring::axis(const double* axis)
			{
				return (const double*) memcpy(m_axis, axis, 3 * sizeof(double));
			}
			const double* Spring::axis(double t, double a)
			{
				m_axis[0] = cos(t) * cos(a);
				m_axis[1] = sin(t) * cos(a);
				m_axis[2] = sin(a);
				return m_axis;
			}
			const double* Spring::axis(double tx, double ty, double tz)
			{
				m_axis[0] = tx;
				m_axis[1] = ty;
				m_axis[2] = tz;
				return m_axis;
			}
			
			//analysis
			void Spring::apply(void)
			{
				return;
			}
			
			//formulation
			double* Spring::internal_force(double* f) const
			{
				//parameters
				const double cs = m_damping;
				const double ks = m_stiffness;
				//state
				const double ux = node(0)->state(nodes::dof::translation_x);
				const double uy = node(0)->state(nodes::dof::translation_y);
				const double uz = node(0)->state(nodes::dof::translation_z);
				const double vx = node(0)->velocity(nodes::dof::translation_x);
				const double vy = node(0)->velocity(nodes::dof::translation_y);
				const double vz = node(0)->velocity(nodes::dof::translation_z);
				const double us = m_axis[0] * ux + m_axis[1] * uy + m_axis[2] * uz;
				const double vs = m_axis[0] * vx + m_axis[1] * vy + m_axis[2] * vz;
				//internal force
				f[0] = (ks * us + cs * vs) * m_axis[0];
				f[1] = (ks * us + cs * vs) * m_axis[1];
				f[2] = (ks * us + cs * vs) * m_axis[2];
				//return
				return f;
			}
			
			double* Spring::damping(double* c) const
			{
				const double cs = m_damping;
				c[0] = cs * m_axis[0] * m_axis[0];
				c[4] = cs * m_axis[1] * m_axis[1];
				c[8] = cs * m_axis[2] * m_axis[2];
				c[1] = c[3] = cs * m_axis[0] * m_axis[1];
				c[2] = c[6] = cs * m_axis[0] * m_axis[2];
				c[5] = c[7] = cs * m_axis[1] * m_axis[2];
				return c;
			}
			double* Spring::stiffness(double* k) const
			{
				const double ks = m_stiffness;
				k[0] = ks * m_axis[0] * m_axis[0];
				k[4] = ks * m_axis[1] * m_axis[2];
				k[8] = ks * m_axis[1] * m_axis[2];
				k[1] = k[3] = ks * m_axis[0] * m_axis[1];
				k[2] = k[6] = ks * m_axis[0] * m_axis[2];
				k[5] = k[7] = ks * m_axis[1] * m_axis[2];
				return k;
			}
			
			//results
			void Spring::plot(double size, const double* color, const double** positions) const
			{
				return;
			}
		}
	}
}
