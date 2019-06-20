//qt
#include <QStatusBar>
#include <QMainWindow>

//boost
#include <boost/filesystem.hpp>

//mat
#include "misc/util.h"

//fea
#include "Models/Model.h"

#include "Plot/Plot.h"
#include "Plot/What.h"
#include "Plot/Colors.h"

#include "Mesh/Mesh.h"
#include "Mesh/Nodes/Dofs.h"
#include "Mesh/Nodes/Node.h"
#include "Mesh/Elements/States.h"
#include "Mesh/Elements/Element.h"

//gui
#include "Canvas/Elements.h"

#include "Actions/Results/Elements.h"

namespace gui
{
	namespace canvas
	{
		//constructors
		Elements::Elements(QWidget* parent) : Model(parent), m_steps(0), m_states(0), 
		m_results(nullptr),
		m_parameter(nullptr), 
		m_positions(nullptr), 
		m_step{nullptr, nullptr},
		m_limits{nullptr, nullptr},
		m_element{nullptr, nullptr}
		{
			return;
		}
	
		//destructor
		Elements::~Elements(void)
		{
			//sizes
			const unsigned ns = m_steps;
			const unsigned nn = m_model->mesh()->nodes();
			const unsigned ne = m_model->mesh()->elements();
			const unsigned nt = mat::bit_find(m_states, unsigned(fea::mesh::elements::state::last));
			//delete
			for(unsigned i = 0; i < ns; i++)
			{
				for(unsigned j = 0; j < nn; j++)
				{
					delete[] m_positions[i][j];
				}
				for(unsigned j = 0; j < nt; j++)
				{
					for(unsigned k = 0; k < ne; k++)
					{
						delete[] m_results[i][j][k];
					}
					delete[] m_results[i][j];
				}
				delete[] m_results[i];
				delete[] m_positions[i];
			}
			delete[] m_step[0];
			delete[] m_step[1];
			delete[] m_node[0];
			delete[] m_node[1];
			delete[] m_results;
			delete[] m_limits[0];
			delete[] m_limits[1];
			delete[] m_parameter;
			delete[] m_positions;
			delete[] m_element[0];
			delete[] m_element[1];
		}
		
		//draw
		void Elements::paintGL(void) 
		{
			//bound
			m_bound.apply();
			//clear
			resizeGL(width(), height());
			glClear(GL_COLOR_BUFFER_BIT);
			//step
			const unsigned s = ((results::Elements*) parent())->step();
			//dof
			const unsigned j = ((results::Elements*) parent())->state();
			//paths
			glColor4dv(m_model->plot()->colors()->paths());
			for(const unsigned i : m_paths)
			{
				glBegin(GL_LINE_STRIP);
				for(unsigned j = 0; j <= s; j++)
				{
					glVertex3dv(m_positions[j][i]);
				}
				glEnd();
			}
			//plots
			for(const unsigned i : m_plots)
			{
				m_model->plot(m_bound.size() / m_bound.zoom(), (const double**) m_positions[i], nullptr, (const double**) m_results[i][j]);
			}
			//plot
			if(std::find(m_plots.begin(), m_plots.end(), s) == m_plots.end())
			{
				m_model->plot(m_bound.size() / m_bound.zoom(), (const double**) m_positions[s], nullptr, (const double**) m_results[s][j]);
			}
			//axes
			if(m_model->plot()->what()->axes())
			{
				draw_axes();
			}
			//scale
			if(m_model->plot()->what()->scale())
			{
				draw_scale();
			}
			//update
			update();
		}
		
		//data
		void Elements::allocate(void)
		{
			//solver
			const std::string path_load = m_model->path() + "/" + m_model->name() + "/Solver/Load.txt";
			const std::string path_time = m_model->path() + "/" + m_model->name() + "/Solver/Time.txt";
			const std::string path_stif = m_model->path() + "/" + m_model->name() + "/Solver/Stiffness.txt";
			const std::string path_freq = m_model->path() + "/" + m_model->name() + "/Solver/Frequencies.txt";
			const std::string path_parm = 
				boost::filesystem::exists(path_load) ? path_load : 
				boost::filesystem::exists(path_time) ? path_time : 
				boost::filesystem::exists(path_stif) ? path_stif : path_freq;
			//steps
			const unsigned& ns = m_steps = 0;
			FILE* file = fopen(path_parm.c_str(), "r");
			while(!feof(file))
			{
				if(fgetc(file) == '\n')
				{
					m_steps++;
				}
			}
			fclose(file);
			//state
			for(unsigned i = 0; i < m_model->mesh()->elements(); i++)
			{
				m_states |= m_model->mesh()->element(i)->states();
			}
			//sizes
			const unsigned nn = m_model->mesh()->nodes();
			const unsigned ne = m_model->mesh()->elements();
			const unsigned nt = mat::bit_find(m_states, unsigned(fea::mesh::elements::state::last));
			//allocate
			m_step[0] = new unsigned[nt];
			m_step[1] = new unsigned[nt];
			m_node[0] = new unsigned[nt];
			m_node[1] = new unsigned[nt];
			m_limits[0] = new double[nt];
			m_limits[1] = new double[nt];
			m_parameter = new double[ns];
			m_results = new double***[ns];
			m_positions = new double**[ns];
			m_element[0] = new unsigned[nt];
			m_element[1] = new unsigned[nt];
			for(unsigned i = 0; i < ns; i++)
			{
				m_results[i] = new double**[nt];
				m_positions[i] = new double*[nn];
				for(unsigned j = 0; j < nn; j++)
				{
					m_positions[i][j] = new double[6];
				}
				for(unsigned j = 0; j < nt; j++)
				{
					m_results[i][j] = new double*[ne];
					for(unsigned k = 0; k < ne; k++)
					{
						m_results[i][j][k] = new double[m_model->mesh()->element(k)->nodes()];
					}
				}
			}
		}
		void Elements::get_data(void)
		{
			//sizes
			double u, s;
			const unsigned ns = m_steps;
			const unsigned nn = m_model->mesh()->nodes();
			const unsigned ne = m_model->mesh()->elements();
			const unsigned nd = mat::log2(unsigned(fea::mesh::nodes::dof::last)) + 1;
			const unsigned nt = mat::log2(unsigned(fea::mesh::elements::state::last)) + 1;
			const unsigned np = mat::bit_find(m_states, unsigned(fea::mesh::elements::state::last));
			//status
			QStatusBar* status = ((QMainWindow*) parent()->parent())->statusBar();
			//solver
			const std::string path_load = m_model->path() + "/" + m_model->name() + "/Solver/Load.txt";
			const std::string path_time = m_model->path() + "/" + m_model->name() + "/Solver/Time.txt";
			const std::string path_stif = m_model->path() + "/" + m_model->name() + "/Solver/Stiffness.txt";
			const std::string path_freq = m_model->path() + "/" + m_model->name() + "/Solver/Frequencies.txt";
			const std::string path_parm = 
				boost::filesystem::exists(path_load) ? path_load : 
				boost::filesystem::exists(path_time) ? path_time : 
				boost::filesystem::exists(path_stif) ? path_stif : path_freq;
			//dofs
			const unsigned dm = 
				(unsigned) fea::mesh::nodes::dof::rotation_x |
				(unsigned) fea::mesh::nodes::dof::rotation_y |
				(unsigned) fea::mesh::nodes::dof::rotation_z |
				(unsigned) fea::mesh::nodes::dof::translation_x |
				(unsigned) fea::mesh::nodes::dof::translation_y |
				(unsigned) fea::mesh::nodes::dof::translation_z;
			//load parameter
			FILE* file = fopen(path_parm.c_str(), "r");
			for(unsigned i = 0; i < ns; i++)
			{
				fscanf(file, "%lf", &m_parameter[i]);
			}
			fclose(file);
			//load nodes
			for(unsigned i = 0; i < nn; i++)
			{
				//node
				const double* x = m_model->mesh()->node(i)->coordinates();
				const unsigned dt = m_model->mesh()->node(i)->dof_types();
				//open file
				char path[200], msg[200];
				const std::string folder = m_model->path() + "/" + m_model->name() + "/State";
				sprintf(path, "%s/N%04d.txt", folder.c_str(), i);
				FILE* file = fopen(path, "r");
				//read file
				for(unsigned j = 0; j < ns; j++)
				{
					for(unsigned k = 0; k < nd; k++)
					{
						if(1 << k & dt)
						{
							fscanf(file, "%lf", &u);
						}
						if(1 << k & dm)
						{
							m_positions[j][i][k] = (k < 3 ? x[k] : 0) + (1 << k & dt ? u : 0);
						}
					}
				}
				//close file
				fclose(file);
				sprintf(msg, "Elements: Loading Nodes (%5.2lf\%)", 100 * double(i) / nn);
				status->showMessage(msg);
			}
			//load elements
			for(unsigned i = 0; i < ne; i++)
			{
				//states
				const unsigned nn = m_model->mesh()->element(i)->nodes();
				const unsigned st = m_model->mesh()->element(i)->states();
				//open file
				char p, path[200], msg[200];
				const std::string folder = m_model->path() + "/" + m_model->name() + "/Elements";
				sprintf(path, "%s/E%04d.txt", folder.c_str(), i);
				FILE* file = fopen(path, "r");
				//read file
				for(unsigned j = 0; j < ns; j++)
				{
					for(unsigned k = 0; k < nn; k++)
					{
						for(unsigned p = 0; p < nt; p++)
						{
							if(1 << p & st)
							{
								fscanf(file, "%lf", &s);
							}
							if(1 << p & m_states)
							{
								m_results[j][mat::bit_find(m_states, 1 << p)][i][k] = 1 << p & st ? s : 0;
							}
						}
					}
				}
				//close file
				fclose(file);
				sprintf(msg, "Elements: Loading Elements (%5.2lf\%)", 100 * double(i) / ne);
				status->showMessage(msg);
			}
			//scale
			for(unsigned i = 0; i < np; i++)
			{
				m_step[0][i] = m_step[1][i] = 0;
				m_node[0][i] = m_node[1][i] = 0;
				m_element[0][i] = m_element[1][i] = 0;
				double& vi = m_limits[0][i] = m_results[0][i][0][0];
				double& vj = m_limits[1][i] = m_results[0][i][0][0];
				for(unsigned j = 0; j < ns; j++)
				{
					for(unsigned k = 0; k < ne; k++)
					{
						for(unsigned p = 0; p < m_model->mesh()->element(k)->nodes(); p++)
						{
							if(vi > m_results[j][i][k][p])
							{
								m_step[0][i] = j;
								m_node[0][i] = p;
								m_element[0][i] = k;
								vi = m_results[j][i][k][p];
							}
							if(vj < m_results[j][i][k][p])
							{
								m_step[1][i] = j;
								m_node[1][i] = p;
								m_element[1][i] = k;
								vj = m_results[j][i][k][p];
							}
						}
					}
				}
				for(unsigned j = 0; j < ns; j++)
				{
					for(unsigned k = 0; k < ne; k++)
					{
						for(unsigned p = 0; p < m_model->mesh()->element(k)->nodes(); p++)
						{
							m_results[j][i][k][p] = vi == vj ? 0.50 : (m_results[j][i][k][p] - vi) / (vj - vi);
						}
					}
				}
			}
			//print
			status->showMessage("Elements: Loading Complete!", 5000);
		}
			
		//update
		void Elements::update_model(fea::models::Model* model)
		{
			//model
			m_model = model;
			//data
			allocate();
			get_data();
			//bound
			m_bound.update((const double***) m_positions, m_steps, model->mesh()->nodes());
		}
		
		//events
		void Elements::mousePressEvent(QMouseEvent* event)
		{
			//base call
			Model::mousePressEvent(event);
			//set focus
			((QWidget*) parent())->setFocus(Qt::MouseFocusReason);
		}
	}
}
