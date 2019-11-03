//std
#include <cstdio>

//qt
#include <QStatusBar>
#include <QMainWindow>
#include <QApplication>

//mat
#include "misc/util.h"

//fea
#include "Model/Model.h"

#include "Mesh/Mesh.h"
#include "Mesh/Nodes/Node.h"
#include "Mesh/Nodes/Dofs.h"
#include "Mesh/Joints/Joint.h"
#include "Mesh/Joints/States.h"
#include "Mesh/Elements/States.h"
#include "Mesh/Elements/Element.h"

#include "Boundary/Boundary.h"
#include "Boundary/Supports/Support.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Solver.h"
#include "Analysis/Solvers/Watch_Dof.h"

//gui
#include "Results/Path.h"

namespace gui
{
	namespace results
	{
		//constructors
		Path::Path(const fea::models::Model* model) : 
			m_model(model), m_steps(0), 
			m_dof(nullptr), m_step(nullptr), m_solver(nullptr),
			m_support(nullptr), m_joint(nullptr), m_element(nullptr)
		
		{
			m_node[0] = nullptr;
			m_node[1] = nullptr;
			m_node[2] = nullptr;
		}
		
		//destructor
		Path::~Path(void)
		{
			clear();
		}
		
		//data
		unsigned Path::steps(void) const
		{
			return m_steps;
		}
		
		const double* Path::dof(void) const
		{
			return m_dof;
		}
		const double* Path::step(void) const
		{
			return m_step;
		}
		const double* Path::solver(void) const
		{
			return m_solver;
		}
		
		const double** Path::support(void) const
		{
			return (const double**) m_support;
		}
		
		const double**** Path::node(void) const
		{
			return (const double****) m_node;
		}
		const double**** Path::joint(void) const
		{
			return (const double****) m_joint;
		}
		const double**** Path::element(void) const
		{
			return (const double****) m_element;
		}
		
		void Path::model(const fea::models::Model* model)
		{
			m_model = model;
		}
		
		//data
		bool Path::read(void)
		{
			allocate();
			return read_node() && read_solver() && read_joint() && read_element() && read_support();
		}
		void Path::clear(void)
		{
			clear_node();
			clear_joint();
			clear_solver();
			clear_element();
			clear_support();
		}
		
		//data
		void Path::setup(void)
		{
			//files
			FILE* load = fopen((m_model->folder() + "/Solver/Load.txt").c_str(), "r");
			FILE* time = fopen((m_model->folder() + "/Solver/Time.txt").c_str(), "r");
			//check
			if(!load && !time)
			{
				return;
			}
			//steps
			m_steps = 0;
			FILE* file = load ? load : time;
			while(!feof(file))
			{
				if(fgetc(file) == '\n')
				{
					m_steps++;
				}
			}
			fclose(file);
		}
		void Path::allocate(void)
		{
			setup();
			allocate_node();
			allocate_joint();
			allocate_solver();
			allocate_element();
			allocate_support();
		}
	
		//read
		bool Path::read_node(void)
		{
			//data
			char path[200];
			const unsigned ns = m_steps;
			const unsigned nn = m_model->mesh()->nodes().size();
			const char* name[] = {"State", "Velocity", "Acceleration"};
			const unsigned wn = m_model->analysis()->solver()->watch_dof()->m_node;
			const unsigned wt = (unsigned) m_model->analysis()->solver()->watch_dof()->m_dof;
			const unsigned wd = mat::bit_index(m_model->mesh()->node(wn)->dof_types(), wt);
			//read
			for(unsigned i = 0; i < 3; i++)
			{
				if(m_node[i])
				{
					for(unsigned j = 0; j < nn; j++)
					{
						//size
						const unsigned nd = m_model->mesh()->node(j)->dofs();
						//path
						sprintf(path, "%s/%s/N%04d.txt", m_model->folder().c_str(), name[i], j);
						//file
						FILE* file = fopen(path, "r");
						if(!file)
						{
							return false;
						}
						//read
						for(unsigned k = 0; k < ns; k++)
						{
							for(unsigned p = 0; p < nd; p++)
							{
								if(fscanf(file, "%lf", &m_node[i][j][p][k]) != 1)
								{
									return false;
								}
							}
						}
						//close
						fclose(file);
					}
				}
			}
			//dof
			for(unsigned i = 0; i < ns; i++)
			{
				m_dof[i] = m_node[0][wn][wd][i];
			}
			//return
			return true;
		}
		bool Path::read_joint(void)
		{
			//data
			char path[200];
			const unsigned ns = m_steps;
			const unsigned nj = m_model->mesh()->joints().size();
			//read
			for(unsigned i = 0; i < nj; i++)
			{
				//sizes
				const unsigned nn = m_model->mesh()->joint(i)->nodes();
				const unsigned nr = mat::bit_count(m_model->mesh()->joint(i)->states());
				//path
				sprintf(path, "%s/Joints/J%04d.txt", m_model->folder().c_str(), i);
				//file
				FILE* file = fopen(path, "r");
				if(!file)
				{
					return false;
				}
				//read
				for(unsigned p = 0; p < ns; p++)
				{
					for(unsigned k = 0; k < nn; k++)
					{
						for(unsigned j = 0; j < nr; j++)
						{
							if(fscanf(file, "%lf", &m_joint[i][j][k][p]) != 1)
							{
								return false;
							}
						}
					}
				}
				//close
				fclose(file);
			}
			//return
			return true;
		}
		bool Path::read_solver(void)
		{
			//files
			FILE* load = fopen((m_model->folder() + "/Solver/Load.txt").c_str(), "r");
			FILE* time = fopen((m_model->folder() + "/Solver/Time.txt").c_str(), "r");
			if(!load && !time)
			{
				return false;
			}
			//solver
			FILE* file = load ? load : time;
			for(unsigned i = 0; i < m_steps; i++)
			{
				m_step[i] = i;
				if(fscanf(file, "%lf", &m_solver[i]) != 1)
				{
					return false;
				}
			}
			//close
			fclose(file);
			//return
			return true;
		}
		bool Path::read_element(void)
		{
			//data
			char path[200];
			const unsigned ns = m_steps;
			const unsigned ne = m_model->mesh()->elements().size();
			//read
			for(unsigned i = 0; i < ne; i++)
			{
				//sizes
				const unsigned nn = m_model->mesh()->element(i)->nodes().size();
				const unsigned nr = mat::bit_count(m_model->mesh()->element(i)->states());
				//path
				sprintf(path, "%s/Elements/E%04d.txt", m_model->folder().c_str(), i);
				//file
				FILE* file = fopen(path, "r");
				if(!file)
				{
					return false;
				}
				//read
				for(unsigned p = 0; p < ns; p++)
				{
					for(unsigned k = 0; k < nn; k++)
					{
						for(unsigned j = 0; j < nr; j++)
						{
							if(fscanf(file, "%lf", &m_element[i][j][k][p]) != 1)
							{
								return false;
							}
						}
					}
				}
				//close
				fclose(file);
			}
			//return
			return true;
		}
		bool Path::read_support(void)
		{
			//data
			char path[200];
			const unsigned ns = m_steps;
			const unsigned nr = m_model->boundary()->supports().size();
			//read
			for(unsigned i = 0; i < nr; i++)
			{
				//path
				sprintf(path, "%s/Supports/S%04d.txt", m_model->folder().c_str(), i);
				//file
				FILE* file = fopen(path, "r");
				if(!file)
				{
					return false;
				}
				//read
				for(unsigned j = 0; j < ns; j++)
				{
					if(fscanf(file, "%lf", &m_support[i][j]) != 1)
					{
						return false;
					}
				}
				//close
				fclose(file);
			}
			//return
			return true;
		}
		
		//clear
		void Path::clear_node(void)
		{
			for(unsigned i = 0; i < 3; i++)
			{
				if(m_node[i])
				{
					for(unsigned j = 0; j < m_model->mesh()->nodes().size(); j++)
					{
						for(unsigned k = 0; k < m_model->mesh()->node(j)->dofs(); k++)
						{
							delete[] m_node[i][j][k];
						}
						delete[] m_node[i][j];
					}
					delete[] m_node[i];
					m_node[i] = nullptr;
				}
			}
		}
		void Path::clear_joint(void)
		{
			//data
			const unsigned nj = m_model->mesh()->joints().size();
			//clear
			if(m_joint)
			{
				for(unsigned i = 0; i < nj; i++)
				{
					const unsigned nn = m_model->mesh()->joint(i)->nodes();
					const unsigned nr = mat::bit_count(m_model->mesh()->joint(i)->states());
					for(unsigned j = 0; j < nr; j++)
					{
						for(unsigned k = 0; k < nn; k++)
						{
							delete[] m_joint[i][j][k];
						}
						delete[] m_joint[i][j];
					}
					delete[] m_joint[i];
				}
				delete[] m_joint;
				m_joint = nullptr;
			}
		}
		void Path::clear_solver(void)
		{
			//dof
			delete[] m_dof;
			m_dof = nullptr;
			//step
			delete[] m_step;
			m_step = nullptr;
			//solver
			delete[] m_solver;
			m_solver = nullptr;
		}
		void Path::clear_element(void)
		{
			//data
			const unsigned ne = m_model->mesh()->elements().size();
			//clear
			if(m_element)
			{
				for(unsigned i = 0; i < ne; i++)
				{
					const unsigned nn = m_model->mesh()->element(i)->nodes().size();
					const unsigned nr = mat::bit_count(m_model->mesh()->element(i)->states());
					for(unsigned j = 0; j < nr; j++)
					{
						for(unsigned k = 0; k < nn; k++)
						{
							delete[] m_element[i][j][k];
						}
						delete[] m_element[i][j];
					}
					delete[] m_element[i];
				}
				delete[] m_element;
				m_element = nullptr;
			}
		}
		void Path::clear_support(void)
		{
			if(m_support)
			{
				for(unsigned i = 0; i < m_model->boundary()->supports().size(); i++)
				{
					delete[] m_support[i];
				}
				delete[] m_support;
				m_support = nullptr;
			}
		}
		
		//allocate
		void Path::allocate_node(void)
		{
			//data
			const unsigned nn = m_model->mesh()->nodes().size();
			const unsigned ss = m_model->analysis()->solver()->state_set();
			//allocate
			for(unsigned i = 0; i < 3; i++)
			{
				if(ss & 1 << i)
				{
					m_node[i] = new double**[nn];
					for(unsigned j = 0; j < nn; j++)
					{
						m_node[i][j] = new double*[m_model->mesh()->node(j)->dofs()];
						for(unsigned k = 0; k < m_model->mesh()->node(j)->dofs(); k++)
						{
							m_node[i][j][k] = new double[m_steps];
						}
					}
				}
			}
		}
		void Path::allocate_joint(void)
		{
			//data
			const unsigned nj = m_model->mesh()->joints().size();
			//allocate
			m_joint = new double***[nj];
			for(unsigned i = 0; i < nj; i++)
			{
				const unsigned nn = m_model->mesh()->joint(i)->nodes();
				const unsigned ns = mat::bit_count(m_model->mesh()->joint(i)->states());
				m_joint[i] = new double**[ns];
				for(unsigned j = 0; j < ns; j++)
				{
					m_joint[i][j] = new double*[nn];
					for(unsigned k = 0; k < nn; k++)
					{
						m_joint[i][j][k] = new double[m_steps];
					}
				}
			}
		}
		void Path::allocate_solver(void)
		{
			m_dof = new double[m_steps];
			m_step = new double[m_steps];
			m_solver = new double[m_steps];
		}
		void Path::allocate_element(void)
		{
			//data
			const unsigned ne = m_model->mesh()->elements().size();
			//allocate
			m_element = new double***[ne];
			for(unsigned i = 0; i < ne; i++)
			{
				const unsigned nn = m_model->mesh()->element(i)->nodes().size();
				const unsigned ns = mat::bit_count(m_model->mesh()->element(i)->states());
				m_element[i] = new double**[ns];
				for(unsigned j = 0; j < ns; j++)
				{
					m_element[i][j] = new double*[nn];
					for(unsigned k = 0; k < nn; k++)
					{
						m_element[i][j][k] = new double[m_steps];
					}
				}
			}
		}
		void Path::allocate_support(void)
		{
			//data
			const unsigned ns = m_model->boundary()->supports().size();
			//allocate
			m_support = new double*[ns];
			for(unsigned i = 0; i < ns; i++)
			{
				m_support[i] = new double[m_steps];
			}
		}
	}
}
