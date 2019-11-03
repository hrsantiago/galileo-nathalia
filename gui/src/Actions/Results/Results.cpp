////mat
//#include "misc/util.h"

////fea
//#include "Model/Model.h"

//#include "Mesh/Mesh.h"
//#include "Mesh/Nodes/Node.h"
//#include "Mesh/Nodes/Dofs.h"
//#include "Mesh/Joints/Joint.h"
//#include "Mesh/Joints/States.h"
//#include "Mesh/Elements/States.h"
//#include "Mesh/Elements/Element.h"

//#include "Boundary/Boundary.h"

//#include "Analysis/Analysis.h"
//#include "Analysis/Solvers/Solver.h"

////gui
//#include "Actions/Results/Results.h"

//namespace gui
//{
//	//constructors
//	Results::Results(fea::models::Model* model) : 
//		m_model(model), m_steps(0), m_dofs(0), m_states(0), 
//		m_step(nullptr), m_solver(nullptr), m_joint(nullptr), 
//		m_node(nullptr), m_element(nullptr), m_support(nullptr), m_position(nullptr)
//	{
//		return;
//	}

//	//destructor
//	Results::~Results(void)
//	{
//		delete_node();
//		delete_joint();
//		delete_solver();
//		delete_element();
//		delete_support();
//		delete_position();
//	}
//	
//	//data
//	unsigned Results::steps(void) const
//	{
//		return m_steps;
//	}
//		
//	const double* Results::step(void) const
//	{
//		return m_step;
//	}
//	const double* Results::solver(void) const
//	{
//		return m_solver;
//	}
//	
//	const double** Results::support(void) const
//	{
//		return (const double**) m_support;
//	}
//	
//	const double*** Results::position(void) const
//	{
//		return (const double***) m_position;
//	}
//	
//	const double**** Results::node(void) const
//	{
//		return (const double****) m_node;
//	}
//	const double**** Results::joint(void) const
//	{
//		return (const double****) m_joint;
//	}
//	const double**** Results::element(void) const
//	{
//		return (const double****) m_element;
//	}

//	//read
//	void Results::read_solver(void)
//	{
//		//check
//		FILE* load = fopen((m_model->folder() + "/Solver/Load.txt").c_str(), "r");
//		FILE* time = fopen((m_model->folder() + "/Solver/Time.txt").c_str(), "r");
//		if(!load && !time)
//		{
//			return;
//		}
//		//file
//		FILE* file = load ? load : time;
//		//steps
//		while(!feof(file))
//		{
//			if(fgetc(file) == '\n')
//			{
//				m_steps++;
//			}
//		}
//		//parameter
//		fseek(file, 0, SEEK_SET);
//		m_step = new double[m_steps];
//		m_solver = new double[m_steps];
//		for(unsigned i = 0; i < m_steps; i++)
//		{
//			m_step[i] = i;
//			fscanf(file, "%lf", &m_solver[i]);
//		}
//		fclose(file);
//	}
//	
//	void Results::read_node(void)
//	{
//		//states
//		const unsigned ss[] = {
//			(unsigned) fea::analysis::solvers::state::u,
//			(unsigned) fea::analysis::solvers::state::v,
//			(unsigned) fea::analysis::solvers::state::a
//		};
//		for(unsigned i = 0; i < 3; i++)
//		{
//			if(!m_state && m_model->analysis()->solver()->state_set() & (unsigned) fea::analysis::solvers::state::u)
//			{
//				//data
//				char path[200];
//				const unsigned ns = m_steps;
//				const unsigned nn = m_model->mesh()->nodes();
//				const unsigned nd = mat::log2(unsigned(fea::mesh::nodes::dof::last)) + 1;
//				//position dofs
//				const unsigned dm = 
//					(unsigned) fea::mesh::nodes::dof::rotation_x |
//					(unsigned) fea::mesh::nodes::dof::rotation_y |
//					(unsigned) fea::mesh::nodes::dof::rotation_z |
//					(unsigned) fea::mesh::nodes::dof::translation_x |
//					(unsigned) fea::mesh::nodes::dof::translation_y |
//					(unsigned) fea::mesh::nodes::dof::translation_z;
//				//allocate
//				m_state = new double**[nn];
//				m_position = new double**[ns];
//				for(unsigned i = 0; i < nn; i++)
//				{
//					m_state[i] = new double*[m_model->mesh()->node(i)->dofs()];
//					for(unsigned j = 0; j < m_model->mesh()->node(i)->dofs(); j++)
//					{
//						m_state[i][j] = new double[ns];
//					}
//				}
//				for(unsigned i = 0; i < ns; i++)
//				{
//					for(unsigned j = 0; j < nn; j++)
//					{
//						m_position[i][j] = new double[6];
//					}
//				}
//				//read
//				for(unsigned i = 0; i < nn; i++)
//				{
//					//node
//					const double* x = m_model->mesh()->node(i)->coordinates();
//					const unsigned dt = m_model->mesh()->node(i)->dof_types();
//					//path
//					sprintf(path, "%s/%s/State/N%04d.txt", m_model->path().c_str(), m_model->name().c_str(), i);
//					//file
//					FILE* file = fopen(path, "r");
//					for(unsigned j = 0; j < ns; j++)
//					{
//						for(unsigned k = 0, p = 0; k < nd; k++)
//						{
//							if(1 << k & dt)
//							{
//								fscanf(file, "%lf", &m_state[i][p++][j]);
//							}
//							if(1 << k & dm)
//							{
//								m_position[j][i][k] = (k < 3 ? x[k] : 0) + (1 << k & dt ? m_state[i][p - 1][j] : 0);
//							}
//						}
//					}
//					fclose(file);
//				}
//			}
//		}
//	}
//	void Results::read_velocity(void)
//	{
//		if(!m_velocity && m_model->analysis()->solver()->state_set() & (unsigned) fea::analysis::solvers::state::v)
//		{
//			//data
//			char path[200];
//			const unsigned ns = m_steps;
//			const unsigned nn = m_model->mesh()->nodes();
//			//allocate
//			m_velocity = new double**[nn];
//			for(unsigned i = 0; i < nn; i++)
//			{
//				m_velocity[i] = new double*[m_model->mesh()->node(i)->dofs()];
//				for(unsigned j = 0; j < m_model->mesh()->node(i)->dofs(); j++)
//				{
//					m_velocity[i][j] = new double[ns];
//				}
//			}
//			//read
//			for(unsigned i = 0; i < nn; i++)
//			{
//				//path
//				sprintf(path, "%s/%s/Velocity/N%04d.txt", m_model->path().c_str(), m_model->name().c_str(), i);
//				//file
//				FILE* file = fopen(path, "r");
//				for(unsigned j = 0; j < ns; j++)
//				{
//					for(unsigned k = 0; k < m_model->mesh()->node(i)->dofs(); k++)
//					{
//						fscanf(file, "%lf", &m_velocity[i][k][j]);
//					}
//				}
//				fclose(file);
//			}
//		}
//	}
//	void Results::read_acceleration(void)
//	{
//		if(!m_acceleration && m_model->analysis()->solver()->state_set() & (unsigned) fea::analysis::solvers::state::a)
//		{
//			//data
//			char path[200];
//			const unsigned ns = m_steps;
//			const unsigned nn = m_model->mesh()->nodes();
//			//allocate
//			m_acceleration = new double**[nn];
//			for(unsigned i = 0; i < nn; i++)
//			{
//				m_acceleration[i] = new double*[m_model->mesh()->node(i)->dofs()];
//				for(unsigned j = 0; j < m_model->mesh()->node(i)->dofs(); j++)
//				{
//					m_acceleration[i][j] = new double[ns];
//				}
//			}
//			//read
//			for(unsigned i = 0; i < nn; i++)
//			{
//				//path
//				sprintf(path, "%s/%s/Acceleration/N%04d.txt", m_model->path().c_str(), m_model->name().c_str(), i);
//				//file
//				FILE* file = fopen(path, "r");
//				for(unsigned j = 0; j < ns; j++)
//				{
//					for(unsigned k = 0; k < m_model->mesh()->node(i)->dofs(); k++)
//					{
//						fscanf(file, "%lf", &m_acceleration[i][k][j]);
//					}
//				}
//				fclose(file);
//			}
//		}
//	}
//	
//	void Results::read_joint(void)
//	{
//		
//	}
//	void Results::read_element(void)
//	{
//		
//	}
//	void Results::read_support(void)
//	{
//		
//	}
//	
//	//delete
//	void Results::delete_step(void)
//	{
//		delete[] m_step;
//	}
//	void Results::delete_solver(void)
//	{
//		delete[] m_solver;
//	}
//	
//	void Results::delete_node(void)
//	{
//		for(unsigned i = 0; i < 3; i++)
//		{
//			if(m_node[i])
//			{
//				for(unsigned i = 0; i < m_model->mesh()->nodes(); i++)
//				{
//					for(unsigned j = 0; j < m_model->mesh()->node(i)->dofs(); j++)
//					{
//						delete[] m_state[i][j];
//					}
//					delete[] m_state[i];
//				}
//				delete[] m_state;
//			}
//		}
//	}
//	void Results::delete_position(void)
//	{
//		if(m_position)
//		{
//			for(unsigned i = 0; i < m_steps; i++)
//			{
//				for(unsigned j = 0; j < m_model->mesh()->nodes(); j++)
//				{
//					delete[] m_position[i][j];
//				}
//				delete[] m_position[i];
//			}
//			delete[] m_position;
//		}
//	}
//	void Results::delete_velocity(void)
//	{
//		if(m_velocity)
//		{
//			for(unsigned i = 0; i < m_model->mesh()->nodes(); i++)
//			{
//				for(unsigned j = 0; j < m_model->mesh()->node(i)->dofs(); j++)
//				{
//					delete[] m_velocity[i][j];
//				}
//				delete[] m_velocity[i];
//			}
//			delete[] m_velocity;
//		}
//	}
//	void Results::delete_acceleration(void)
//	{
//		if(m_acceleration)
//		{
//			for(unsigned i = 0; i < m_model->mesh()->nodes(); i++)
//			{
//				for(unsigned j = 0; j < m_model->mesh()->node(i)->dofs(); j++)
//				{
//					delete[] m_acceleration[i][j];
//				}
//				delete[] m_acceleration[i];
//			}
//			delete[] m_acceleration;
//		}
//	}
//	
//	void Results::delete_joint(void)
//	{
//		if(m_joint)
//		{
//			for(unsigned i = 0; i < m_model->mesh()->joints(); i++)
//			{
//				const unsigned nn = m_model->mesh()->joint(i)->nodes();
//				const unsigned ls = unsigned(fea::mesh::joints::state::last);
//				const unsigned nr = mat::bit_find(m_model->mesh()->joint(i)->states(), ls);
//				for(unsigned j = 0; j < nr; j++)
//				{
//					for(unsigned k = 0; k < nn; k++)
//					{
//						delete[] m_joint[i][j][k];
//					}
//					delete[] m_joint[i][j];
//				}
//				delete[] m_joint[i];
//			}
//			delete[] m_joint;
//		}
//	}
//	void Results::delete_element(void)
//	{
//		if(m_element)
//		{
//			for(unsigned i = 0; i < m_model->mesh()->elements(); i++)
//			{
//				const unsigned nn = m_model->mesh()->element(i)->nodes();
//				const unsigned ls = unsigned(fea::mesh::elements::state::last);
//				const unsigned nr = mat::bit_find(m_model->mesh()->element(i)->states(), ls);
//				for(unsigned j = 0; j < nr; j++)
//				{
//					for(unsigned k = 0; k < nn; k++)
//					{
//						delete[] m_element[i][j][k];
//					}
//					delete[] m_element[i][j];
//				}
//				delete[] m_element[i];
//			}
//			delete[] m_element;
//		}
//	}
//	void Results::delete_support(void)
//	{
//		if(m_support)
//		{
//			for(unsigned i = 0; i < m_model->boundary()->supports(); i++)
//			{
//				delete[] m_support[i];
//			}
//			delete[] m_support;
//		}
//	}
//}
