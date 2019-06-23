//std
#include <cstring>
#include <GL/gl.h>

//mat
#include "linear/dense.h"
#include "misc/rotation.h"

//fea
#include "Mesh/Mesh.h"

#include "Mesh/Nodes/Dofs.h"
#include "Mesh/Nodes/Node.h"

#include "Mesh/Joints/Types.h"
#include "Mesh/Joints/Rigid.h"
#include "Mesh/Joints/States.h"

#include "Boundary/Dependencies/Dependency.h"

namespace fea
{
	namespace mesh
	{
		namespace joints
		{
			//constructors
			Rigid::Rigid(void)
			{
				return;
			}

			//destructor
			Rigid::~Rigid(void)
			{
				return;
			}

			//types
			unsigned Rigid::states(void) const
			{
				return 0;
			}
			joints::type Rigid::type(void) const
			{
				return joints::type::rigid;
			}
			
			//dofs
			std::vector<unsigned> Rigid::dofs(void) const
			{
				return std::vector<unsigned>(m_nodes.size(), 0);
			}
			
			//analysis
			void Rigid::apply(void)
			{
				return;
			}
			
			void Rigid::configure(void)
			{
				for(unsigned i = 1; i < m_nodes.size(); i++)
				{
					//create
					boundary::Dependency* dux = create();
					boundary::Dependency* duy = create();
					boundary::Dependency* duz = create();
					boundary::Dependency* dtx = create();
					boundary::Dependency* dty = create();
					boundary::Dependency* dtz = create();
					//position
					const double* p0 = node(0)->coordinates();
					const double* pi = node(i)->coordinates();
					//configure
					dux->slave_node(m_nodes[i]);
					duy->slave_node(m_nodes[i]);
					duz->slave_node(m_nodes[i]);
					dtx->slave_node(m_nodes[i]);
					dty->slave_node(m_nodes[i]);
					dtz->slave_node(m_nodes[i]);
					dux->master_node(0, m_nodes[0]);
					duy->master_node(0, m_nodes[0]);
					duz->master_node(0, m_nodes[0]);
					dtx->master_node(0, m_nodes[0]);
					dty->master_node(0, m_nodes[0]);
					dtz->master_node(0, m_nodes[0]);
					dtx->slave_dof(nodes::dof::rotation_x);
					dty->slave_dof(nodes::dof::rotation_y);
					dtz->slave_dof(nodes::dof::rotation_z);
					dux->slave_dof(nodes::dof::translation_x);
					duy->slave_dof(nodes::dof::translation_y);
					duz->slave_dof(nodes::dof::translation_z);
					dtx->master_dof(0, nodes::dof::rotation_x);
					dty->master_dof(0, nodes::dof::rotation_y);
					dtz->master_dof(0, nodes::dof::rotation_z);
					dux->master_dof(0, nodes::dof::translation_x);
					duy->master_dof(0, nodes::dof::translation_y);
					duz->master_dof(0, nodes::dof::translation_z);
					dux->add_master(m_nodes[0], nodes::dof::rotation_x);
					dux->add_master(m_nodes[0], nodes::dof::rotation_y);
					dux->add_master(m_nodes[0], nodes::dof::rotation_z);
					duy->add_master(m_nodes[0], nodes::dof::rotation_x);
					duy->add_master(m_nodes[0], nodes::dof::rotation_y);
					duy->add_master(m_nodes[0], nodes::dof::rotation_z);
					duz->add_master(m_nodes[0], nodes::dof::rotation_x);
					duz->add_master(m_nodes[0], nodes::dof::rotation_y);
					duz->add_master(m_nodes[0], nodes::dof::rotation_z);
					//states
					std::function<double(double*, unsigned)> fu = [p0, pi] (double* d, unsigned i) 
					{
						double R[9], u = d[0];
						mat::rotation_matrix(R, d + 1);
						for(unsigned j = 0; j < 3; j++)
						{
							u += ((i == j) - R[i + 3 * j]) * (p0[j] - pi[j]);
						}
						return u;
					};
					dux->state([fu] (double* d) { return fu(d, 0); });
					duy->state([fu] (double* d) { return fu(d, 1); });
					duz->state([fu] (double* d) { return fu(d, 2); });
					//gradients
					std::function<double(double*, unsigned, unsigned)> gu = [p0, pi] (double* d, unsigned i, unsigned j)
					{
						double R[9], x[3], y[3];
						mat::multiply(y, mat::rotation_matrix(R, d + 1), mat::sub(x, pi, p0, 3), 3, 3);
						switch(i)
						{
							case 0: return (double) 1;
							case 1: return j == 0 ? 0 : j == 1 ? -y[2] : +y[1];
							case 2: return j == 1 ? 0 : j == 2 ? -y[0] : +y[2];
							case 3: return j == 2 ? 0 : j == 0 ? -y[1] : +y[0];
							default: return (double) 0;
						}
					};
					dux->gradient([gu] (double* d, unsigned i) { return gu(d, i, 0); });
					duy->gradient([gu] (double* d, unsigned i) { return gu(d, i, 1); });
					duz->gradient([gu] (double* d, unsigned i) { return gu(d, i, 2); });
				}
			}
			
			//formulation
			double* Rigid::internal_force(double* f) const
			{
				return f;
			}
			
			double* Rigid::damping(double* c) const
			{
				return c;
			}
			double* Rigid::stiffness(double* k) const
			{
				return k;
			}
			
			//results
			void Rigid::plot(double size, const double* color, const double** positions) const
			{
				//setup
				glLineWidth(2);
				glColor4dv(color);
				//plot
				glBegin(GL_LINES);
				for(unsigned i = 1; i < m_nodes.size(); i++)
				{
					glVertex3dv(positions ? positions[m_nodes[0]] : node(0)->coordinates());
					glVertex3dv(positions ? positions[m_nodes[i]] : node(i)->coordinates());
				}
				glEnd();
			}
		}
	}
}
