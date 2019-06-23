//std
#include <cstring>
#include <GL/gl.h>
#include <algorithm>

//mat
#include "misc/util.h"
#include "linear/dense.h"
#include "misc/rotation.h"

//fea
#include "Model/Model.h"

#include "Mesh/Mesh.h"
#include "Mesh/Nodes/Dofs.h"
#include "Mesh/Nodes/Node.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Solver.h"
#include "Analysis/Assembler/Assembler.h"

namespace fea
{
	namespace mesh
	{
		namespace nodes
		{
			//constructors
			Node::Node(void) : 
				m_dof_types(0),
				m_state_old(nullptr), m_velocity_old(nullptr), m_acceleration_old(nullptr),
				m_state_new(nullptr), m_velocity_new(nullptr), m_acceleration_new(nullptr)
			{
				memset(m_coordinates, 0, 3 * sizeof(double));
				memset(m_rotation_old, 0, 9 * sizeof(double));
				memset(m_rotation_new, 0, 9 * sizeof(double));
				m_rotation_old[0] = m_rotation_old[4] = m_rotation_old[8] = 1;
				m_rotation_new[0] = m_rotation_new[4] = m_rotation_new[8] = 1;
			}

			//destructor
			Node::~Node(void)
			{
				delete[] m_state_old;
				delete[] m_state_new;
				delete[] m_velocity_old;
				delete[] m_velocity_new;
				delete[] m_acceleration_new;
				delete[] m_acceleration_old;
			}

			//serialization
			void Node::load(FILE* file)
			{
				//load from file
				fscanf(file, "%lf %lf %lf %d", &m_coordinates[0], &m_coordinates[1], &m_coordinates[2], &m_dof_types);
			}
			void Node::save(FILE* file) const
			{
				fprintf(file, "%+.6e %+.6e %+.6e %03d", m_coordinates[0], m_coordinates[1], m_coordinates[2], m_dof_types);
			}

			//data
			Mesh* Node::mesh(void)
			{
				return m_mesh;
			}
			
			const double* Node::coordinates(void) const
			{
				return m_coordinates;
			}
			const double* Node::coordinates(const double* coordinates)
			{
				return (const double*) memcpy(m_coordinates, coordinates, 3 * sizeof(double));
			}
			const double* Node::coordinates(double v, unsigned i)
			{
				m_coordinates[i] = v;
				return m_coordinates;
			}
			const double* Node::coordinates(double x, double y, double z)
			{
				m_coordinates[0] = x;
				m_coordinates[1] = y;
				m_coordinates[2] = z;
				return m_coordinates;
			}

			//index
			unsigned Node::index(void) const
			{
				return distance(m_mesh->m_nodes.begin(), find(m_mesh->m_nodes.begin(), m_mesh->m_nodes.end(), this));
			}

			//moviment
			double* Node::position(double* x) const
			{
				memcpy(x, m_coordinates, 3 * sizeof(double));
				for(unsigned i = 0; i < 3; i++)
				{
					if(unsigned(dof::translation_x) << i & m_dof_types)
					{
						x[i] += m_state_new[mat::bit_find(m_dof_types, unsigned(dof::translation_x) << i)];
					}
					
				}
				return x;
			}
			double* Node::rotation(double* t, unsigned c) const
			{
				const double* d[] = {m_state_new, m_velocity_new, m_acceleration_new};
				for(unsigned i = 0; i < 3; i++)
				{
					const unsigned j = unsigned(dof::rotation_x) << i;
					t[i] = d[c] && j & m_dof_types ? d[c][mat::bit_find(m_dof_types, j)] : 0;
				}
				return t;
			}			
			double* Node::displacement(double* u, unsigned c) const
			{
				const double* d[] = {m_state_new, m_velocity_new, m_acceleration_new};
				for(unsigned i = 0; i < 3; i++)
				{
					const unsigned j = unsigned(dof::translation_x) << i;
					u[i] = d[c] && j & m_dof_types ? d[c][mat::bit_find(m_dof_types, j)] : 0;
				}
				return u;
			}
			
			const double* Node::rotation_tensor(void) const
			{
				return m_rotation_new;
			}

			//state
			double Node::state(dof type, unsigned c) const
			{
				const unsigned char p = mat::bit_find(m_dof_types, unsigned(type));
				if(m_state_new && m_dof_types & unsigned(type))
				{
					switch(c)
					{
						case 0: return m_state_old[p];
						case 1: return m_state_new[p];
						case 2: return m_state_new[p] - m_state_old[p];
					}
				}
				return 0;
			}
			double Node::velocity(dof type, unsigned c) const
			{
				const unsigned char p = mat::bit_find(m_dof_types, unsigned(type));
				if(m_velocity_new && m_dof_types & unsigned(type))
				{
					switch(c)
					{
						case 0: return m_velocity_old[p];
						case 1: return m_velocity_new[p];
						case 2: return m_velocity_new[p] - m_velocity_old[p];
					}
				}
				return 0;
			}
			double Node::acceleration(dof type, unsigned c) const
			{
				const unsigned char p = mat::bit_find(m_dof_types, unsigned(type));
				if(m_acceleration_new && m_dof_types & unsigned(type))
				{
					switch(c)
					{
						case 0: return m_acceleration_old[p];
						case 1: return m_acceleration_new[p];
						case 2: return m_acceleration_new[p] - m_acceleration_old[p];
					}
				}
				return 0;
			}

			//dof
			const char* Node::dof_name(nodes::dof dof)
			{
				switch(dof)
				{
					case nodes::dof::translation_x:
						return "ux";
					case nodes::dof::translation_y:
						return "uy";
					case nodes::dof::translation_z:
						return "uz";
					case nodes::dof::rotation_x:
						return "rx";
					case nodes::dof::rotation_y:
						return "ry";
					case nodes::dof::rotation_z:
						return "rz";
					case nodes::dof::temperature:
						return "Te";
					default:
						return "error";
				}
			}
			
			unsigned Node::dofs(void) const
			{
				return mat::bit_find(m_dof_types, (unsigned) dof::last);
			}
			unsigned Node::dof_index(dof d) const
			{
				return m_dof_types & (unsigned) d ? m_dof[mat::bit_find(m_dof_types, (unsigned) d)] : 0;
			}
			unsigned Node::dof_index(unsigned index) const
			{
				return m_dof[index];
			}
			unsigned Node::dof_types(void) const
			{
				return m_dof_types;
			}

			//analysis
			void Node::add_dof_type(dof type)
			{
				m_dof_types |= (unsigned) type;
			}
			void Node::add_dof_type(unsigned types)
			{
				m_dof_types |= types;
			}
			
			bool Node::check(void) const
			{
				return true;
			}
			void Node::prepare(unsigned& counter)
			{
				//dof size
				const char n_dof = mat::bit_find(m_dof_types, (unsigned) nodes::dof::last);
				//set dof
				m_dof.resize(n_dof);
				for(unsigned i = 0; i < (unsigned) n_dof; i++)
				{
					m_dof[i] = counter + i;
				}
				counter += n_dof;
				//data set
				const unsigned solver_set = m_mesh->model()->analysis()->solver()->state_set();
				const unsigned data_set[] = {
					(unsigned) analysis::solvers::state::u,
					(unsigned) analysis::solvers::state::v,
					(unsigned) analysis::solvers::state::a
				};
				//reference
				double** data_old[] = {&m_state_old, &m_velocity_old, &m_acceleration_old};
				double** data_new[] = {&m_state_new, &m_velocity_new, &m_acceleration_new};
				//initialize
				for(unsigned i = 0; i < 3; i++)
				{
					if(solver_set & data_set[i])
					{
						delete[] *data_old[i];
						delete[] *data_new[i];
						*data_old[i] = new double[n_dof];
						*data_new[i] = new double[n_dof];
						memset(*data_old[i], 0, n_dof * sizeof(double));
						memset(*data_new[i], 0, n_dof * sizeof(double));
					}
				}
			}
			
			void Node::finish(void) const
			{
				//path
				const std::string folder = m_mesh->model()->folder();
				const std::string state_name[3] = {"State", "Velocity", "Acceleration"};
				//state set
				const unsigned solver_set = m_mesh->model()->analysis()->solver()->state_set();
				const unsigned data_set[] = {
					(unsigned) analysis::solvers::state::u,
					(unsigned) analysis::solvers::state::v,
					(unsigned) analysis::solvers::state::a
				};
				//flush
				for(unsigned i = 0; i < 3; i++)
				{
					if(solver_set & data_set[i])
					{
						char path[200];
						sprintf(path, "%s/%s/N%04d.txt", folder.c_str(), state_name[i].c_str(), index());
						FILE* file = fopen(path, "w");
						fprintf(file, m_results[i].c_str());
						fclose(file);
					}
				}
			}
			void Node::record(void)
			{
				//reference
				double* state[] = {m_state_new, m_velocity_new, m_acceleration_new};
				//sets
				const unsigned solver_set = m_mesh->model()->analysis()->solver()->state_set();
				const unsigned data_set[] = {
					(unsigned) analysis::solvers::state::u,
					(unsigned) analysis::solvers::state::v,
					(unsigned) analysis::solvers::state::a
				};
				//save results
				char formatter[200];
				for(int i = 0; i < 3; i++)
				{
					if(solver_set & data_set[i])
					{
						for(unsigned j = 0; j < m_dof.size(); j++)
						{
							sprintf(formatter, "%+.6e ", state[i][j]);
							m_results[i] += formatter;
						}
						m_results[i] += "\n";
					}
				}
			}
			void Node::update(void)
			{
				//state set
				const unsigned solver_set = m_mesh->model()->analysis()->solver()->state_set();
				const unsigned data_set[] = {
					(unsigned) analysis::solvers::state::u,
					(unsigned) analysis::solvers::state::v,
					(unsigned) analysis::solvers::state::a
				};
				//reference
				double* data_old[] = {m_state_old, m_velocity_old, m_acceleration_old};
				double* data_new[] = {m_state_new, m_velocity_new, m_acceleration_new};
				//dof
				const unsigned n_dof = (unsigned) m_dof.size();
				//update
				for(unsigned i = 0; i < 3; i++)
				{
					if(solver_set & data_set[i])
					{
						memcpy(data_old[i], data_new[i], n_dof * sizeof (double));
					}
				}
				memcpy(m_rotation_old, m_rotation_new, 9 * sizeof (double));
			}
			void Node::restore(void)
			{
				//state set
				const unsigned solver_set = m_mesh->model()->analysis()->solver()->state_set();
				const unsigned state_set[] = {
					(unsigned) analysis::solvers::state::u,
					(unsigned) analysis::solvers::state::v,
					(unsigned) analysis::solvers::state::a
				};
				//reference
				double* data_old[] = {m_state_old, m_velocity_old, m_acceleration_old};
				double* data_new[] = {m_state_new, m_velocity_new, m_acceleration_new};
				//dof
				const unsigned n_dof = (unsigned) m_dof.size();
				//restore
				for(unsigned i = 0; i < 3; i++)
				{
					if(solver_set & state_set[i])
					{
						memcpy(data_new[i], data_old[i], n_dof * sizeof (double));
					}
				}
				memcpy(m_rotation_new, m_rotation_old, 9 * sizeof (double));
			}
			
			void Node::update_rotation(void)
			{
				//increment rotation
				const unsigned dofs_rotation = 
					(unsigned) dof::rotation_x | 
					(unsigned) dof::rotation_y | 
					(unsigned) dof::rotation_z;
				if((m_dof_types & dofs_rotation) == dofs_rotation)
				{
					mat::rotation_matrix(m_rotation_new, m_state_new + mat::bit_find(m_dof_types, (unsigned) dof::rotation_x));
				}
			}

			//apply
			void Node::apply_state(void)
			{
				const double* u = m_mesh->model()->analysis()->solver()->state();
				const unsigned nu = m_mesh->model()->analysis()->assembler()->dof_unknow(); 
				for(unsigned i = 0; i < m_dof.size(); i++)
				{
					if(m_dof[i] < nu)
					{
						m_state_new[i] = u[m_dof[i]];
					}
				}
			}
			void Node::apply_velocity(void)
			{
				const double* v = m_mesh->model()->analysis()->solver()->velocity();
				const unsigned nu = m_mesh->model()->analysis()->assembler()->dof_unknow(); 
				for(unsigned i = 0; i < m_dof.size(); i++)
				{
					if(m_dof[i] < nu)
					{
						m_velocity_new[i] = v[m_dof[i]];
					}
				}
			}
			void Node::apply_acceleration(void)
			{
				const double* a = m_mesh->model()->analysis()->solver()->acceleration();
				const unsigned nu = m_mesh->model()->analysis()->assembler()->dof_unknow(); 
				for(unsigned i = 0; i < m_dof.size(); i++)
				{
					if(m_dof[i] < nu)
					{
						m_acceleration_new[i] = a[m_dof[i]];
					}
				}
			}

			//increment
			void Node::increment_state(void)
			{
				//solver increment
				const double* du = m_mesh->model()->analysis()->solver()->state_increment();
				//increment state
				const unsigned nu = m_mesh->model()->analysis()->assembler()->dof_unknow();
				for(unsigned i = 0; i < m_dof.size(); i++)
				{
					if(m_dof[i] < nu)
					{
						m_state_new[i] = m_state_old[i] + du[m_dof[i]];
					}
				}
				//increment rotation
				const unsigned dofs_rotation = 
					(unsigned) dof::rotation_x | 
					(unsigned) dof::rotation_y | 
					(unsigned) dof::rotation_z;
				if((m_dof_types & dofs_rotation) == dofs_rotation)
				{
					//index
					const char dp = mat::bit_find(m_dof_types, (unsigned) dof::rotation_x);
					//increment
					double dt[3], dR[9];
					dt[0] = m_dof[dp + 0] < nu ? du[m_dof[dp + 0]] : 0;
					dt[1] = m_dof[dp + 1] < nu ? du[m_dof[dp + 1]] : 0;
					dt[2] = m_dof[dp + 2] < nu ? du[m_dof[dp + 2]] : 0;
					//increment state
					mat::rotation_vector(m_state_new + dp, mat::multiply(m_rotation_new, mat::rotation_matrix(dR, dt), m_rotation_old, 3, 3, 3));
				}
			}
			void Node::increment_velocity(void)
			{
				const unsigned nu = m_mesh->model()->analysis()->assembler()->dof_unknow(); 
				const double* dv = m_mesh->model()->analysis()->solver()->velocity_increment();
				for(unsigned i = 0; i < m_dof.size(); i++)
				{
					if(m_dof[i] < nu)
					{
						m_velocity_new[i] = m_velocity_old[i] + dv[m_dof[i]];
					}
				}
			}
			void Node::increment_acceleration(void)
			{
				const unsigned nu = m_mesh->model()->analysis()->assembler()->dof_unknow(); 
				const double* da = m_mesh->model()->analysis()->solver()->acceleration_increment();
				for(unsigned i = 0; i < m_dof.size(); i++)
				{
					if(m_dof[i] < nu)
					{
						m_acceleration_new[i] = m_acceleration_old[i] + da[m_dof[i]];
					}
				}
			}

			void Node::plot(const double* color, const double* position) const
			{
				glPointSize(7);
				glColor4dv(color);
				glBegin(GL_POINTS);
				glVertex3dv(position ? position : m_coordinates);
				glEnd();
			}

			//static attributes
			Mesh* Node::m_mesh = nullptr;
		}
	}
}
