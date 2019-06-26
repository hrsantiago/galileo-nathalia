//std
#include <cmath>

//qt
#include <QStatusBar>
#include <QMainWindow>

//boost
#include <boost/filesystem.hpp>

//mat
#include "misc/util.h"

//fea
#include "Model/Model.h"

#include "Plot/Plot.h"
#include "Plot/What.h"
#include "Plot/Colors.h"

#include "Mesh/Mesh.h"
#include "Mesh/Nodes/Dofs.h"
#include "Mesh/Nodes/Node.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Solver.h"
#include "Analysis/Solvers/Watch_Dof.h"

//gui
#include "Canvas/DeformedCanvas.h"

#include "Actions/Results/Deformed.h"

namespace gui
{
	namespace canvas
	{
		//constructors
		Deformed::Deformed(QWidget* parent) : Model(parent), m_steps(0), m_dof(nullptr), m_parameter(nullptr), m_positions(nullptr)
		{
			return;
		}
	
		//destructor
		Deformed::~Deformed(void)
		{
			//sizes
			const unsigned ns = m_steps;
			const unsigned nn = m_model->mesh()->nodes();
			//delete
			for(unsigned i = 0; i < ns; i++)
			{
				for(unsigned j = 0; j < nn; j++)
				{
					delete[] m_positions[i][j];
				}
				delete[] m_positions[i];
			}
			delete[] m_dof;
			delete[] m_parameter;
			delete[] m_positions;
		}
		
		//draw
		void Deformed::paintGL(void) 
		{
			//bound
			m_bound.apply();
			//clear
			resizeGL(width(), height());
			glClear(GL_COLOR_BUFFER_BIT);
			//step
			const unsigned s = ((results::Deformed*) parent())->step();
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
				m_model->plot(m_bound.size() / m_bound.zoom(), (const double**) m_positions[i]);
			}
			//plot
			if(std::find(m_plots.begin(), m_plots.end(), s) == m_plots.end())
			{
				m_model->plot(m_bound.size() / m_bound.zoom(), (const double**) m_positions[s]);
			}
			//axis
			if(m_model->plot()->what()->axes())
			{
				draw_axes();
			}
			draw_nodes();
			//update
			update();
		}
		
		//data
		void Deformed::allocate(void)
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
			//nodes
			const unsigned nn = m_model->mesh()->nodes();
			//allocate
			m_dof = new double[ns];
			m_parameter = new double[ns];
			m_positions = new double**[ns];
			for(unsigned i = 0; i < ns; i++)
			{
				m_positions[i] = new double*[nn];
				for(unsigned j = 0; j < nn; j++)
				{
					m_positions[i][j] = new double[6];
				}
			}
		}
		void Deformed::get_data(void)
		{
			//sizes
			double u;
			const unsigned ns = m_steps;
			const unsigned nn = m_model->mesh()->nodes();
			const unsigned nw = m_model->analysis()->solver()->watch_dof()->m_node;
			const unsigned nd = mat::log2(unsigned(fea::mesh::nodes::dof::last)) + 1;
			const unsigned dw = (unsigned) m_model->analysis()->solver()->watch_dof()->m_dof;
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
						if(i == nw && 1U << k == dw)
						{
							m_dof[j] = (dt & dw) ? u : 0;
						}
					}
				}
				//close file
				fclose(file);
				sprintf(msg, "Deformed: Loading Node (%5.2lf\%)", 100 * double(i) / nn);
				status->showMessage(msg);
			}
			//print
			status->showMessage("Deformed: Loading Complete!", 5000);
		}
			
		//update
		void Deformed::update_model(fea::models::Model* model)
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
		void Deformed::mousePressEvent(QMouseEvent* event)
		{
			//base call
			Model::mousePressEvent(event);
			//set focus
			((QWidget*) parent())->setFocus(Qt::MouseFocusReason);
		}
		
		//slots
		void Deformed::slot_center(void)
		{
			//step
			const unsigned s = ((results::Deformed*) parent())->step();
			//local limits
			double xi[3], xj[3], xc[3], xp[3];
			memcpy(xi, m_positions[s][0], 3 * sizeof(double));
			memcpy(xj, m_positions[s][0], 3 * sizeof(double));
			for(unsigned i = 1; i < m_model->mesh()->nodes(); i++)
			{
				for(unsigned j = 0; j < 3; j++)
				{
					xi[j] = std::min(xi[j], m_positions[s][i][j]);
					xj[j] = std::max(xj[j], m_positions[s][i][j]);
				}
			}
			//pan
			double d = 0;
			m_bound.center(xc);
			for(unsigned i = 0; i < 3; i++)
			{
				d += pow(xj[i] - xi[i], 2);
				xp[i] = xc[i] - (xj[i] + xi[i]) / 2;
			}
			m_bound.pan(xp);
			m_bound.zoom(m_bound.size() / sqrt(d));
		}
	}
}
