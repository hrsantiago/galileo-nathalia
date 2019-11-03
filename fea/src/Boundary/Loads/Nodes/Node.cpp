//std
#include <cmath>
#include <GL/gl.h>

//mat
#include "misc/util.h"

//fea
#include "Model/Model.h"

#include "Mesh/Mesh.h"
#include "Mesh/Nodes/Dofs.h"
#include "Mesh/Nodes/Node.h"

#include "Boundary/Boundary.h"
#include "Boundary/Cases/Load_Case.h"
#include "Boundary/Loads/Nodes/Node.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Solver.h"

namespace fea
{
	namespace boundary
	{
		namespace loads
		{
			//constructors
			Node::Node(void)
			{
				return;
			}

			//destructor
			Node::~Node(void)
			{
				return;
			}

			//serialization
			void Node::load(FILE* file)
			{
				Load::load(file);
				fscanf(file, "%d %d", &m_node, &m_dof_type);
			}
			void Node::save(FILE* file) const
			{
				Load::save(file);
				fprintf(file, "%04d %02d ", m_node, m_dof_type);
			}

			//data
			mesh::nodes::dof Node::dof(void) const
			{
				return m_dof_type;
			}
			mesh::nodes::dof Node::dof(mesh::nodes::dof dof_type)
			{
				return m_dof_type = dof_type;
			}
			mesh::nodes::Node* Node::node(void) const
			{
				return m_boundary->model()->mesh()->node(m_node);
			}
			mesh::nodes::Node* Node::node(unsigned node)
			{
				return m_boundary->model()->mesh()->node(m_node = node);
			}

			//index
			unsigned Node::index(void) const
			{
				const unsigned k = m_boundary->model()->analysis()->solver()->load_case();
				for(unsigned i = 0; i < m_boundary->load_case(k)->loads_nodes(); i++)
				{
					if(this == m_boundary->load_case(k)->load_node(i))
					{
						return i;
					}
				}
				return 0;
			}

			//analysis
			bool Node::check(void) const
			{
				const unsigned k = m_boundary->model()->analysis()->solver()->load_case();
				if(m_node >= m_boundary->model()->mesh()->nodes().size())
				{
					printf("Load case %02d: Node load %02d has out of range node\n", k, index());
					return false;
				}
				return true;
			}
			void Node::prepare(void)
			{
				const mesh::nodes::Node* node = m_boundary->model()->mesh()->node(m_node);
				const unsigned char p = mat::bit_index(node->m_dof_types, (unsigned) m_dof_type);
				m_dof = node->m_dof[p];
			}
			void Node::add_dof(void) const
			{
				m_boundary->model()->mesh()->node(m_node)->add_dof_type(m_dof_type);
			}
			
			//results
			void Node::plot(double s, const double* c, const double** p) const
			{
				//position
				const double* x = p ? p[m_node] : node()->coordinates();
				//plot
				switch(m_dof_type)
				{
					case mesh::nodes::dof::rotation_x:
						plot_rotation_x(s, c, x);
						break;
					case mesh::nodes::dof::rotation_y:
						plot_rotation_y(s, c, x);
						break;
					case mesh::nodes::dof::rotation_z:
						plot_rotation_z(s, c, x);
						break;
					case mesh::nodes::dof::translation_x:
						plot_translation_x(s, c, x);
						break;
					case mesh::nodes::dof::translation_y:
						plot_translation_y(s, c, x);
						break;
					case mesh::nodes::dof::translation_z:
						plot_translation_z(s, c, x);
						break;
					case mesh::nodes::dof::temperature:
						break;
					default:
						break;
				}
			}

			void Node::plot_rotation_x(double s, const double* c, const double* p) const
			{
				//sign
				const int d = mat::sign(m_value);
				const double t1 =  30 * M_PI / 180;
				const double t2 = 330 * M_PI / 180;
				//plot
				glColor4dv(c);
				glBegin(GL_LINES);
				glVertex3d(p[0], p[1], p[2]);
				glVertex3d(p[0] - d * s, p[1], p[2]);
				glVertex3d(p[0], p[1], p[2]);
				glVertex3d(p[0] - 0.30 * d * s * cos(t1), p[1] - 0.30 * d * s * sin(t1), p[2]);
				glVertex3d(p[0], p[1], p[2]);
				glVertex3d(p[0] - 0.30 * d * s * cos(t2), p[1] - 0.30 * d * s * sin(t2), p[2]);
				glVertex3d(p[0], p[1], p[2]);
				glVertex3d(p[0] - 0.30 * d * s * cos(t1), p[1], p[2] - 0.30 * d * s * sin(t1));
				glVertex3d(p[0], p[1], p[2]);
				glVertex3d(p[0] - 0.30 * d * s * cos(t2), p[1], p[2] - 0.30 * d * s * sin(t2));
				glVertex3d(p[0] - 0.50 * d * s, p[1], p[2]);
				glVertex3d(p[0] - 0.50 * d * s - 0.30 * d * s * cos(t1), p[1] - 0.30 * d * s * sin(t1), p[2]);
				glVertex3d(p[0] - 0.50 * d * s, p[1], p[2]);
				glVertex3d(p[0] - 0.50 * d * s - 0.30 * d * s * cos(t2), p[1] - 0.30 * d * s * sin(t2), p[2]);
				glVertex3d(p[0] - 0.50 * d * s, p[1], p[2]);
				glVertex3d(p[0] - 0.50 * d * s - 0.30 * d * s * cos(t1), p[1], p[2] - 0.30 * d * s * sin(t1));
				glVertex3d(p[0] - 0.50 * d * s, p[1], p[2]);
				glVertex3d(p[0] - 0.50 * d * s - 0.30 * d * s * cos(t2), p[1], p[2] - 0.30 * d * s * sin(t2));
				glEnd();
			}
			void Node::plot_rotation_y(double s, const double* c, const double* p) const
			{
				//sign
				const int d = mat::sign(m_value);
				const double t1 =  60 * M_PI / 180;
				const double t2 = 120 * M_PI / 180;
				//plot
				glColor4dv(c);
				glBegin(GL_LINES);
				glVertex3d(p[0], p[1], p[2]);
				glVertex3d(p[0], p[1] - d * s, p[2]);
				glVertex3d(p[0], p[1], p[2]);
				glVertex3d(p[0] - 0.30 * d * s * cos(t1), p[1] - 0.30 * d * s * sin(t1), p[2]);
				glVertex3d(p[0], p[1], p[2]);
				glVertex3d(p[0] - 0.30 * d * s * cos(t2), p[1] - 0.30 * d * s * sin(t2), p[2]);
				glVertex3d(p[0], p[1], p[2]);
				glVertex3d(p[0], p[1] - 0.30 * d * s * sin(t1), p[2] - 0.30 * d * s * cos(t1));
				glVertex3d(p[0], p[1], p[2]);
				glVertex3d(p[0], p[1] - 0.30 * d * s * sin(t2), p[2] - 0.30 * d * s * cos(t2));
				glVertex3d(p[0], p[1] - 0.50 * d * s, p[2]);
				glVertex3d(p[0], p[1] - 0.50 * d * s - d * s, p[2]);
				glVertex3d(p[0], p[1] - 0.50 * d * s, p[2]);
				glVertex3d(p[0] - 0.30 * d * s * cos(t1), p[1] - 0.50 * d * s - 0.30 * d * s * sin(t1), p[2]);
				glVertex3d(p[0], p[1] - 0.50 * d * s, p[2]);
				glVertex3d(p[0] - 0.30 * d * s * cos(t2), p[1] - 0.50 * d * s - 0.30 * d * s * sin(t2), p[2]);
				glVertex3d(p[0], p[1] - 0.50 * d * s, p[2]);
				glVertex3d(p[0], p[1] - 0.50 * d * s - 0.30 * d * s * sin(t1), p[2] - 0.30 * d * s * cos(t1));
				glVertex3d(p[0], p[1] - 0.50 * d * s, p[2]);
				glVertex3d(p[0], p[1] - 0.50 * d * s - 0.30 * d * s * sin(t2), p[2] - 0.30 * d * s * cos(t2));
				glEnd();
			}
			void Node::plot_rotation_z(double s, const double* c, const double* p) const
			{
				//sign
				const int d = mat::sign(m_value);
				const double t1 =  60 * M_PI / 180;
				const double t2 = 120 * M_PI / 180;
				//plot
				glColor4dv(c);
				glBegin(GL_LINES);
				glVertex3d(p[0], p[1], p[2]);
				glVertex3d(p[0], p[1], p[2] - d * s);
				glVertex3d(p[0], p[1], p[2]);
				glVertex3d(p[0] - 0.30 * d * s * cos(t1), p[1], p[2] - 0.30 * d * s * sin(t1));
				glVertex3d(p[0], p[1], p[2]);
				glVertex3d(p[0] - 0.30 * d * s * cos(t2), p[1], p[2] - 0.30 * d * s * sin(t2));
				glVertex3d(p[0], p[1], p[2]);
				glVertex3d(p[0], p[1] - 0.30 * d * s * cos(t1), p[2] - 0.30 * d * s * sin(t1));
				glVertex3d(p[0], p[1], p[2]);
				glVertex3d(p[0], p[1] - 0.30 * d * s * cos(t2), p[2] - 0.30 * d * s * sin(t2));
				glVertex3d(p[0], p[1], p[2] - 0.50 * d * s);
				glVertex3d(p[0] - 0.30 * d * s * cos(t1), p[1], p[2] - 0.50 * d * s - 0.30 * d * s * sin(t1));
				glVertex3d(p[0], p[1], p[2] - 0.50 * d * s);
				glVertex3d(p[0] - 0.30 * d * s * cos(t2), p[1], p[2] - 0.50 * d * s - 0.30 * d * s * sin(t2));
				glVertex3d(p[0], p[1], p[2] - 0.50 * d * s);
				glVertex3d(p[0], p[1] - 0.30 * d * s * cos(t1), p[2] - 0.50 * d * s - 0.30 * d * s * sin(t1));
				glVertex3d(p[0], p[1], p[2] - 0.50 * d * s);
				glVertex3d(p[0], p[1] - 0.30 * d * s * cos(t2), p[2] - 0.50 * d * s - 0.30 * d * s * sin(t2));
				glEnd();
			}
			void Node::plot_translation_x(double s, const double* c, const double* p) const
			{
				//sign
				const int d = mat::sign(m_value);
				const double t1 =  30 * M_PI / 180;
				const double t2 = 330 * M_PI / 180;
				//plot
				glColor4dv(c);
				glBegin(GL_LINES);
				glVertex3d(p[0], p[1], p[2]);
				glVertex3d(p[0] - d * s, p[1], p[2]);
				glVertex3d(p[0], p[1], p[2]);
				glVertex3d(p[0] - 0.30 * d * s * cos(t1), p[1] - 0.30 * d * s * sin(t1), p[2]);
				glVertex3d(p[0], p[1], p[2]);
				glVertex3d(p[0] - 0.30 * d * s * cos(t2), p[1] - 0.30 * d * s * sin(t2), p[2]);
				glVertex3d(p[0], p[1], p[2]);
				glVertex3d(p[0] - 0.30 * d * s * cos(t1), p[1], p[2] - 0.30 * d * s * sin(t1));
				glVertex3d(p[0], p[1], p[2]);
				glVertex3d(p[0] - 0.30 * d * s * cos(t2), p[1], p[2] - 0.30 * d * s * sin(t2));
				glEnd();
			}
			void Node::plot_translation_y(double s, const double* c, const double* p) const
			{
				//sign
				const int d = mat::sign(m_value);
				const double t1 =  60 * M_PI / 180;
				const double t2 = 120 * M_PI / 180;
				//plot
				glColor4dv(c);
				glBegin(GL_LINES);
				glVertex3d(p[0], p[1], p[2]);
				glVertex3d(p[0], p[1] - d * s, p[2]);
				glVertex3d(p[0], p[1], p[2]);
				glVertex3d(p[0] - 0.30 * d * s * cos(t1), p[1] - 0.30 * d * s * sin(t1), p[2]);
				glVertex3d(p[0], p[1], p[2]);
				glVertex3d(p[0] - 0.30 * d * s * cos(t2), p[1] - 0.30 * d * s * sin(t2), p[2]);
				glVertex3d(p[0], p[1], p[2]);
				glVertex3d(p[0], p[1] - 0.30 * d * s * sin(t1), p[2] - 0.30 * d * s * cos(t1));
				glVertex3d(p[0], p[1], p[2]);
				glVertex3d(p[0], p[1] - 0.30 * d * s * sin(t2), p[2] - 0.30 * d * s * cos(t2));
				glEnd();
			}
			void Node::plot_translation_z(double s, const double* c, const double* p) const
			{
				//sign
				const int d = mat::sign(m_value);
				const double t1 =  60 * M_PI / 180;
				const double t2 = 120 * M_PI / 180;
				//plot
				glColor4dv(c);
				glBegin(GL_LINES);
				glVertex3d(p[0], p[1], p[2]);
				glVertex3d(p[0], p[1], p[2] - d * s);
				glVertex3d(p[0], p[1], p[2]);
				glVertex3d(p[0] - 0.30 * d * s * cos(t1), p[1], p[2] - 0.30 * d * s * sin(t1));
				glVertex3d(p[0], p[1], p[2]);
				glVertex3d(p[0] - 0.30 * d * s * cos(t2), p[1], p[2] - 0.30 * d * s * sin(t2));
				glVertex3d(p[0], p[1], p[2]);
				glVertex3d(p[0], p[1] - 0.30 * d * s * cos(t1), p[2] - 0.30 * d * s * sin(t1));
				glVertex3d(p[0], p[1], p[2]);
				glVertex3d(p[0], p[1] - 0.30 * d * s * cos(t2), p[2] - 0.30 * d * s * sin(t2));
				glEnd();
			}
		}
	}
}
