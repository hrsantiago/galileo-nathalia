//std
#include <cmath>
#include <GL/gl.h>
#include <algorithm>

//mat
#include "misc/util.h"

//fea
#include "Model/Model.h"

#include "Mesh/Mesh.h"
#include "Mesh/Nodes/Dofs.h"
#include "Mesh/Nodes/Node.h"

#include "Boundary/Boundary.h"
#include "Boundary/Supports/Support.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Solver.h"
#include "Analysis/Assembler/Assembler.h"

namespace fea
{
	namespace boundary
	{
		//construtors
		Support::Support(void) :
			m_inertia(0), m_damping(0), m_stiffness(0), m_reaction(0), 
			m_fix(true), m_node(0), m_dof_type(mesh::nodes::dof::translation_x),
			m_state([](double) { return 0; }), m_velocity([](double) { return 0; }), m_acceleration([](double) { return 0; })
		{
			return;
		}

		//destructor
		Support::~Support(void)
		{
			return;
		}

		//serialization
		void Support::load(FILE* file)
		{
			fscanf(file, "%d %d %d %lf %lf %lf", &m_node, &m_dof_type, &m_fix, &m_inertia, &m_damping, &m_stiffness);
		}
		void Support::save(FILE* file) const
		{
			fprintf(file, "%04d %02d %02d %+.6e %+.6e %+.6e", m_node, m_dof_type, m_fix, m_inertia, m_damping, m_stiffness);
		}
		

		//data
		bool Support::fixed(void) const
		{
			return m_fix;
		}
		bool Support::fixed(bool fixed)
		{
			return m_fix = fixed;
		}
		
		mesh::nodes::dof Support::dof(void) const
		{
			return m_dof_type;
		}
		mesh::nodes::dof Support::dof(mesh::nodes::dof dof_type)
		{
			return m_dof_type = dof_type;
		}
		mesh::nodes::Node* Support::node(void) const
		{
			return m_boundary->model()->mesh()->node(m_node);
		}
		mesh::nodes::Node* Support::node(unsigned node)
		{
			return m_boundary->model()->mesh()->node(m_node = node);
		}
		
		double Support::inertia(void) const
		{
			return m_inertia;
		}
		double Support::inertia(double inertia)
		{
			if(inertia < 0)
			{
				printf("\tWarning: Support inertia must be non negative!\n");
				return 0;
			}
			else
			{
				return m_inertia = inertia;
			}
		}
		double Support::damping(void) const
		{
			return m_damping;
		}
		double Support::damping(double damping)
		{
			if(damping < 0)
			{
				printf("\tWarning: Support damping must be non negative!\n");
				return 0;
			}
			else
			{
				return m_damping = damping;
			}
		}
		double Support::stiffness(void) const
		{
			return m_stiffness;
		}
		double Support::stiffness(double stiffness)
		{
			if(stiffness < 0)
			{
				printf("\tWarning: Support stiffness must be non negative!\n");
				return 0;
			}
			else
			{
				return m_stiffness = stiffness;
			}
		}
		
		std::function<double(double)> Support::state(void) const
		{
			return m_state;
		}
		std::function<double(double)> Support::state(std::function<double(double)>  state)
		{
			return m_state = state;
		}
		std::function<double(double)> Support::velocity(void) const
		{
			return m_velocity;
		}
		std::function<double(double)> Support::velocity(std::function<double(double)> velocity)
		{
			return m_velocity = velocity;
		}
		std::function<double(double)> Support::acceleration(void) const
		{
			return m_acceleration;
		}
		std::function<double(double)> Support::acceleration(std::function<double(double)> acceleration)
		{
			return m_acceleration = acceleration;
		}

		//index
		unsigned Support::index(void) const
		{
			const std::vector<Support*>& list = m_boundary->supports();
			return std::distance(list.begin(), std::find(list.begin(), list.end(), this));
		}
		unsigned Support::index_node(void) const
		{
			return m_node;
		}

		//data
		double Support::state(double t) const
		{
			const mesh::nodes::Node* node = m_boundary->model()->mesh()->node(m_node);
			const unsigned char p = mat::bit_index(node->m_dof_types, (unsigned) m_dof_type);
			return m_fix ? m_state(t) : m_boundary->model()->mesh()->node(m_node)->m_state_new[p];
		}
		double Support::velocity(double t) const
		{
			const mesh::nodes::Node* node = m_boundary->model()->mesh()->node(m_node);
			const unsigned char p = mat::bit_index(node->m_dof_types, (unsigned) m_dof_type);
			return m_fix ? m_velocity(t) : m_boundary->model()->mesh()->node(m_node)->m_velocity_new[p];
		}
		double Support::acceleration(double t) const
		{
			const mesh::nodes::Node* node = m_boundary->model()->mesh()->node(m_node);
			const unsigned char p = mat::bit_index(node->m_dof_types, (unsigned) m_dof_type);
			return m_fix ? m_acceleration(t) : m_boundary->model()->mesh()->node(m_node)->m_acceleration_new[p];
		}

		double Support::reaction(void) const
		{
			return m_reaction;
		}

		//assembler
		void Support::apply_values(void) const
		{
			if(m_fix)
			{
				//node
				const mesh::nodes::Node* node = m_boundary->model()->mesh()->node(m_node);
				//solver
				const analysis::solvers::Solver* solver = m_boundary->model()->analysis()->solver();
				//time
				const double t = solver->time();
				//state set
				const unsigned ss = solver->state_set();
				//apply
				const unsigned char p = mat::bit_index(node->m_dof_types, (unsigned) m_dof_type);
				if(ss & (unsigned) analysis::solvers::state::u)
				{
					node->m_state_new[p] = m_state(t);
				}
				if(ss & (unsigned) analysis::solvers::state::v)
				{
					node->m_velocity_new[p] = m_velocity(t);
				}
				if(ss & (unsigned) analysis::solvers::state::a)
				{
					node->m_acceleration_new[p] = m_acceleration(t);
				}
			}
		}
		void Support::apply_reaction(void)
		{
			//solver
			const analysis::solvers::Solver* solver = m_boundary->model()->analysis()->solver();
			//assembler
			const analysis::Assembler* assembler = m_boundary->model()->analysis()->assembler();
			//reaction
			if(m_fix)
			{
				const double* R = solver->reaction();
				const unsigned nu = assembler->dof_unknow();
				const unsigned nd = assembler->dof_dependent();
				m_reaction = R[m_dof - nu - nd];
			}
			else
			{
				//node
				const mesh::nodes::Node* node = m_boundary->model()->mesh()->node(m_node);
				//properties
				const double m = m_inertia;
				const double c = m_damping;
				const double k = m_stiffness;
				//state set
				const unsigned ss = solver->state_set();
				//reaction
				m_reaction = 0;
				if(ss & (unsigned) analysis::solvers::state::u)
				{
					m_reaction += k * node->state(m_dof_type);
				}
				if(ss & (unsigned) analysis::solvers::state::v)
				{
					m_reaction += c * node->velocity(m_dof_type);
				}
				if(ss & (unsigned) analysis::solvers::state::a)
				{
					m_reaction += m * node->acceleration(m_dof_type);
				}
			}
		}

		//analysis
		bool Support::check(void) const
		{
			//check node
			if(m_node >= m_boundary->model()->mesh()->nodes().size())
			{
				printf("Support %02d has out of range node!\n", index());
				return false;
			}
			//check inertia
			if(m_inertia < 0)
			{
				printf("Support %02d has negative inertia!\n", index());
				return false;
			}
			//check damping
			if(m_damping < 0)
			{
				printf("Support %02d has negative damping!\n", index());
				return false;
			}
			//check stiffness
			if(m_stiffness < 0)
			{
				printf("Support %02d has negative stiffness!\n", index());
				return false;
			}
			//return
			return true;
		}
		void Support::prepare(void)
		{
			mesh::nodes::Node* node = m_boundary->model()->mesh()->node(m_node);
			m_dof = node->m_dof[mat::bit_index(node->m_dof_types, (unsigned) m_dof_type)];
		}
		void Support::record(void)
		{
			char formatter[100];
			sprintf(formatter, "%+.6e\n", m_reaction);
			m_results += formatter;
		}
		void Support::finish(void) const
		{
			//save support data
			char file_path[2000];
			std::string folder_path = m_boundary->model()->path() + "/" + m_boundary->model()->name();
			sprintf(file_path, "%s/Supports/S%04d.txt", folder_path.c_str(), index());
			FILE* file = fopen(file_path, "w");
			fprintf(file, m_results.c_str());
			fclose(file);
		}
		void Support::add_dof(void) const
		{
			m_boundary->model()->mesh()->node(m_node)->add_dof_type(m_dof_type);
		}

		//results
		void Support::plot(double s, const double* c, const double** p) const
		{
			//background
			double b[4];
			glGetDoublev(GL_COLOR_CLEAR_VALUE, b);
			mat::inv_color(b);
			//position
			const double* x = p ? p[m_node] : node()->coordinates();
			//plot
			switch(m_dof_type)
			{
				case mesh::nodes::dof::rotation_x:
					plot_rotation_x(s, c, b, x);
					break;
				case mesh::nodes::dof::rotation_y:
					plot_rotation_y(s, c, b, x);
					break;
				case mesh::nodes::dof::rotation_z:
					plot_rotation_z(s, c, b, x);
					break;
				case mesh::nodes::dof::temperature:
					plot_temperature(s, c, b, x);
					break;
				case mesh::nodes::dof::translation_x:
					plot_translation_x(s, c, b, x);
					break;
				case mesh::nodes::dof::translation_y:
					plot_translation_y(s, c, b, x);
					break;
				case mesh::nodes::dof::translation_z:
					plot_translation_z(s, c, b, x);
					break;
				default:
					break;
			}
		}
		
		void Support::plot_rotation_x(double s, const double* c, const double* b, const double* p) const
		{
			if(m_fix)
			{
				//plot face
				glColor4dv(c);
				glBegin(GL_POLYGON);
				glVertex3d(p[0], p[1] + 0.75 * s * cos( 45 * M_PI / 180), p[2] + 0.75 * s * sin( 45 * M_PI / 180));
				glVertex3d(p[0], p[1] + 0.75 * s * cos(135 * M_PI / 180), p[2] + 0.75 * s * sin(135 * M_PI / 180));
				glVertex3d(p[0], p[1] + 0.75 * s * cos(225 * M_PI / 180), p[2] + 0.75 * s * sin(225 * M_PI / 180));
				glVertex3d(p[0], p[1] + 0.75 * s * cos(315 * M_PI / 180), p[2] + 0.75 * s * sin(315 * M_PI / 180));
				glEnd();
				//plot edge
				glColor4dv(b);
				glBegin(GL_LINE_LOOP);
				glVertex3d(p[0], p[1] + 0.75 * s * cos( 45 * M_PI / 180), p[2] + 0.75 * s * sin( 45 * M_PI / 180));
				glVertex3d(p[0], p[1] + 0.75 * s * cos(135 * M_PI / 180), p[2] + 0.75 * s * sin(135 * M_PI / 180));
				glVertex3d(p[0], p[1] + 0.75 * s * cos(225 * M_PI / 180), p[2] + 0.75 * s * sin(225 * M_PI / 180));
				glVertex3d(p[0], p[1] + 0.75 * s * cos(315 * M_PI / 180), p[2] + 0.75 * s * sin(315 * M_PI / 180));
				glEnd();
			}
			else
			{
				glColor4dv(c);
				glLineWidth(2);
				glBegin(GL_LINE_STRIP);
				const unsigned nl =  5;
				const unsigned np = 80;
				for(unsigned i = 0; i < np; i++)
				{
					const double r = i * s / (np - 1);
					const double t = 2 * nl * M_PI * i / (np - 1);
					glVertex3d(p[0], p[1] + r * sin(t), p[2] - r * cos(t));
				}
				glEnd();
			}
		}
		void Support::plot_rotation_y(double s, const double* c, const double* b, const double* p) const
		{
			if(m_fix)
			{
				//plot face
				glColor4dv(c);
				glBegin(GL_POLYGON);
				glVertex3d(p[0] + 0.75 * s * cos( 45 * M_PI / 180), p[1], p[2] + 0.75 * s * sin( 45 * M_PI / 180));
				glVertex3d(p[0] + 0.75 * s * cos(135 * M_PI / 180), p[1], p[2] + 0.75 * s * sin(135 * M_PI / 180));
				glVertex3d(p[0] + 0.75 * s * cos(225 * M_PI / 180), p[1], p[2] + 0.75 * s * sin(225 * M_PI / 180));
				glVertex3d(p[0] + 0.75 * s * cos(315 * M_PI / 180), p[1], p[2] + 0.75 * s * sin(315 * M_PI / 180));
				glEnd();
				//plot edge
				glColor4dv(b);
				glBegin(GL_LINE_LOOP);
				glVertex3d(p[0] + 0.75 * s * cos( 45 * M_PI / 180), p[1], p[2] + 0.75 * s * sin( 45 * M_PI / 180));
				glVertex3d(p[0] + 0.75 * s * cos(135 * M_PI / 180), p[1], p[2] + 0.75 * s * sin(135 * M_PI / 180));
				glVertex3d(p[0] + 0.75 * s * cos(225 * M_PI / 180), p[1], p[2] + 0.75 * s * sin(225 * M_PI / 180));
				glVertex3d(p[0] + 0.75 * s * cos(315 * M_PI / 180), p[1], p[2] + 0.75 * s * sin(315 * M_PI / 180));
				glEnd();
			}
			else
			{
				glColor4dv(c);
				glLineWidth(2);
				glBegin(GL_LINE_STRIP);
				const unsigned nl =  5;
				const unsigned np = 80;
				for(unsigned i = 0; i < np; i++)
				{
					const double r = i * s / (np - 1);
					const double t = 2 * nl * M_PI * i / (np - 1);
					glVertex3d(p[0] + r * cos(t), p[1], p[2] - r * sin(t));
				}
				glEnd();
			}
		}
		void Support::plot_rotation_z(double s, const double* c, const double* b, const double* p) const
		{
			if(m_fix)
			{
				//plot face
				glColor4dv(c);
				glBegin(GL_POLYGON);
				glVertex3d(p[0] + 0.75 * s * cos( 45 * M_PI / 180), p[1] + 0.75 * s * sin( 45 * M_PI / 180), p[2]);
				glVertex3d(p[0] + 0.75 * s * cos(135 * M_PI / 180), p[1] + 0.75 * s * sin(135 * M_PI / 180), p[2]);
				glVertex3d(p[0] + 0.75 * s * cos(225 * M_PI / 180), p[1] + 0.75 * s * sin(225 * M_PI / 180), p[2]);
				glVertex3d(p[0] + 0.75 * s * cos(315 * M_PI / 180), p[1] + 0.75 * s * sin(315 * M_PI / 180), p[2]);
				glEnd();
				//plot edge
				glColor4dv(b);
				glBegin(GL_LINE_LOOP);
				glVertex3d(p[0] + 0.75 * s * cos( 45 * M_PI / 180), p[1] + 0.75 * s * sin( 45 * M_PI / 180), p[2]);
				glVertex3d(p[0] + 0.75 * s * cos(135 * M_PI / 180), p[1] + 0.75 * s * sin(135 * M_PI / 180), p[2]);
				glVertex3d(p[0] + 0.75 * s * cos(225 * M_PI / 180), p[1] + 0.75 * s * sin(225 * M_PI / 180), p[2]);
				glVertex3d(p[0] + 0.75 * s * cos(315 * M_PI / 180), p[1] + 0.75 * s * sin(315 * M_PI / 180), p[2]);
				glEnd();
			}
			else
			{
				glColor4dv(c);
				glLineWidth(2);
				glBegin(GL_LINE_STRIP);
				const unsigned nl =  5;
				const unsigned np = 80;
				for(unsigned i = 0; i < np; i++)
				{
					const double r = i * s / (np - 1);
					const double t = 2 * nl * M_PI * i / (np - 1);
					glVertex3d(p[0] + r * cos(t), p[1] + r * sin(t), p[2]);
				}
				glEnd();
			}
		}
		void Support::plot_temperature(double s, const double* c, const double* b, const double* p) const
		{
			if(m_fix)
			{
				glColor4dv(c);
				glLineWidth(2);
				glBegin(GL_LINES);
				glVertex3d(p[0], p[1] + s * sin( 45 * M_PI / 180), p[2] - s * cos( 45 * M_PI / 180));
				glVertex3d(p[0], p[1] + s * sin(225 * M_PI / 180), p[2] - s * cos(225 * M_PI / 180));
				glVertex3d(p[0], p[1] + s * sin(135 * M_PI / 180), p[2] - s * cos(135 * M_PI / 180));
				glVertex3d(p[0], p[1] + s * sin(315 * M_PI / 180), p[2] - s * cos(315 * M_PI / 180));
				glVertex3d(p[0] + s * cos( 45 * M_PI / 180), p[1], p[2] - s * sin( 45 * M_PI / 180));
				glVertex3d(p[0] + s * cos(225 * M_PI / 180), p[1], p[2] - s * sin(225 * M_PI / 180));
				glVertex3d(p[0] + s * cos(135 * M_PI / 180), p[1], p[2] - s * sin(135 * M_PI / 180));
				glVertex3d(p[0] + s * cos(315 * M_PI / 180), p[1], p[2] - s * sin(315 * M_PI / 180));
				glVertex3d(p[0] + s * cos( 45 * M_PI / 180), p[1] + s * sin( 45 * M_PI / 180), p[2]);
				glVertex3d(p[0] + s * cos(225 * M_PI / 180), p[1] + s * sin(225 * M_PI / 180), p[2]);
				glVertex3d(p[0] + s * cos(135 * M_PI / 180), p[1] + s * sin(135 * M_PI / 180), p[2]);
				glVertex3d(p[0] + s * cos(315 * M_PI / 180), p[1] + s * sin(315 * M_PI / 180), p[2]);
				glEnd();
			}
			else
			{
				glColor4dv(c);
				glLineWidth(2);
				glBegin(GL_LINE_STRIP);
				const unsigned np = 40;
				for(unsigned i = 0; i < np; i++)
				{
					const double t = 2 * M_PI * i / (np - 1);
					glVertex3d(p[0] + s * cos(t), p[1] + s * sin(t), p[2]);
				}
				glEnd();
				glBegin(GL_LINE_STRIP);
				for(unsigned i = 0; i < np; i++)
				{
					const double t = 2 * M_PI * i / (np - 1);
					glVertex3d(p[0], p[1] + s * sin(t), p[2] - s * cos(t));
				}
				glEnd();
				glBegin(GL_LINE_STRIP);
				for(unsigned i = 0; i < np; i++)
				{
					const double t = 2 * M_PI * i / (np - 1);
					glVertex3d(p[0] + s * cos(t), p[1], p[2] - s * sin(t));
				}
				glEnd();
			}
		}
		void Support::plot_translation_x(double s, const double* c, const double* b, const double* p) const
		{
			if(m_fix)
			{
				//plot face
				glColor4dv(c);
				glBegin(GL_POLYGON);
				glVertex3d(p[0], p[1], p[2]);
				glVertex3d(p[0] + s * cos(150 * M_PI / 180), p[1] + s * sin(150 * M_PI / 180), p[2]);
				glVertex3d(p[0] + s * cos(210 * M_PI / 180), p[1] + s * sin(210 * M_PI / 180), p[2]);
				glEnd();
				//plot edge
				glColor4dv(b);
				glBegin(GL_LINE_LOOP);
				glVertex3d(p[0], p[1], p[2]);
				glVertex3d(p[0] + s * cos(150 * M_PI / 180), p[1] + s * sin(150 * M_PI / 180), p[2]);
				glVertex3d(p[0] + s * cos(210 * M_PI / 180), p[1] + s * sin(210 * M_PI / 180), p[2]);
				glEnd();
			}
			else
			{
				const unsigned n = 5;
				const double ds = s / n;
				glColor4dv(c);
				glLineWidth(2);
				glBegin(GL_LINE_STRIP);
				glVertex3d(p[0], p[1], p[2]);
				for(unsigned i = 0; i < n; i++)
				{
					glVertex3d(p[0] - i * ds - 1 * ds / 4, p[1] - s / 4, p[2]);
					glVertex3d(p[0] - i * ds - 3 * ds / 4, p[1] + s / 4, p[2]);
				}
				glVertex3d(p[0] - s, p[1], p[2]);
				glEnd();
				glBegin(GL_LINE_STRIP);
				glVertex3d(p[0], p[1], p[2]);
				for(unsigned i = 0; i < n; i++)
				{
					glVertex3d(p[0] - i * ds - 1 * ds / 4, p[1], p[2] - s / 4);
					glVertex3d(p[0] - i * ds - 3 * ds / 4, p[1], p[2] + s / 4);
				}
				glVertex3d(p[0] - s, p[1], p[2]);
				glEnd();
			}
		}
		void Support::plot_translation_y(double s, const double* c, const double* b, const double* p) const
		{
			if(m_fix)
			{
				//plot face
				glColor4dv(c);
				glBegin(GL_POLYGON);
				glVertex3d(p[0], p[1], p[2]);
				glVertex3d(p[0] + s * cos(240 * M_PI / 180), p[1] + s * sin(240 * M_PI / 180), p[2]);
				glVertex3d(p[0] + s * cos(300 * M_PI / 180), p[1] + s * sin(300 * M_PI / 180), p[2]);
				glEnd();
				//plot edge
				glColor4dv(b);
				glBegin(GL_LINE_LOOP);
				glVertex3d(p[0], p[1], p[2]);
				glVertex3d(p[0] + s * cos(240 * M_PI / 180), p[1] + s * sin(240 * M_PI / 180), p[2]);
				glVertex3d(p[0] + s * cos(300 * M_PI / 180), p[1] + s * sin(300 * M_PI / 180), p[2]);
				glEnd();
			}
			else
			{
				const unsigned n = 5;
				const double ds = s / n;
				glColor4dv(c);
				glLineWidth(2);
				glBegin(GL_LINE_STRIP);
				glVertex3d(p[0], p[1], p[2]);
				for(unsigned i = 0; i < n; i++)
				{
					glVertex3d(p[0] - s / 4, p[1] - i * ds - 1 * ds / 4, p[2]);
					glVertex3d(p[0] + s / 4, p[1] - i * ds - 3 * ds / 4, p[2]);
				}
				glVertex3d(p[0], p[1] - s, p[2]);
				glEnd();
				glBegin(GL_LINE_STRIP);
				glVertex3d(p[0], p[1], p[2]);
				for(unsigned i = 0; i < n; i++)
				{
					glVertex3d(p[0], p[1] - i * ds - 1 * ds / 4, p[2] - s / 4);
					glVertex3d(p[0], p[1] - i * ds - 3 * ds / 4, p[2] + s / 4);
				}
				glVertex3d(p[0], p[1] - s, p[2]);
				glEnd();
			}
		}
		void Support::plot_translation_z(double s, const double* c, const double* b, const double* p) const
		{
			if(m_fix)
			{
				//plot face
				glColor4dv(c);
				glBegin(GL_POLYGON);
				glVertex3d(p[0], p[1], p[2]);
				glVertex3d(p[0], p[1] + s * cos(240 * M_PI / 180), p[2] + s * sin(240 * M_PI / 180));
				glVertex3d(p[0], p[1] + s * cos(300 * M_PI / 180), p[2] + s * sin(300 * M_PI / 180));
				glEnd();
				//plot line loop
				glColor4dv(b);
				glBegin(GL_LINE_LOOP);
				glVertex3d(p[0], p[1], p[2]);
				glVertex3d(p[0], p[1] + s * cos(240 * M_PI / 180), p[2] + s * sin(240 * M_PI / 180));
				glVertex3d(p[0], p[1] + s * cos(300 * M_PI / 180), p[2] + s * sin(300 * M_PI / 180));
				glEnd();
			}
			else
			{
				const unsigned n = 5;
				const double ds = s / n;
				glColor4dv(c);
				glLineWidth(2);
				glBegin(GL_LINE_STRIP);
				glVertex3d(p[0], p[1], p[2]);
				for(unsigned i = 0; i < n; i++)
				{
					glVertex3d(p[0] - s / 4, p[1], p[2] - i * ds - 1 * ds / 4);
					glVertex3d(p[0] + s / 4, p[1], p[2] - i * ds - 3 * ds / 4);
				}
				glVertex3d(p[0], p[1], p[2] - s);
				glEnd();
				glBegin(GL_LINE_STRIP);
				glVertex3d(p[0], p[1], p[2]);
				for(unsigned i = 0; i < n; i++)
				{
					glVertex3d(p[0], p[1] - s / 4, p[2] - i * ds - 1 * ds / 4);
					glVertex3d(p[0], p[1] + s / 4, p[2] - i * ds - 3 * ds / 4);
				}
				glVertex3d(p[0], p[1], p[2] - s);
				glEnd();
			}
		}

		//static attributes
		Boundary* Support::m_boundary = nullptr;
		std::list<unsigned> Support::m_update_nodes;
	}
}
