//std
#include <cstdlib>

//qt
#include <QKeyEvent>
#include <QStatusBar>
#include <QMessageBox>
#include <QFileDialog>
#include <QMainWindow>

//boost
#include <boost/filesystem.hpp>

//mat
#include "misc/util.h"
#include "linear/dense.h"

//fea
#include "Models/Model.h"

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
#include "Actions/Results/Limits.h"
#include "Actions/Results/Equilibrium_Path.h"

//ui
#include "ui_Equilibrium_Path.h"

namespace gui
{
	namespace results
	{
		//constructors
		Equilibrium_Path::Equilibrium_Path(fea::models::Model* model, QWidget* parent) : QDialog(parent), 
		m_steps(0), 
		m_step(nullptr), m_parameter(nullptr), m_support(nullptr), 
		m_state(nullptr), m_velocity(nullptr), m_acceleration(nullptr), 
		m_joint(nullptr), m_element(nullptr), 
		m_ui(new Ui::Equilibrium_Path), m_model(model)
		{
			//data
			prepare();
			allocate();
			get_data();
			slot_fit();
			//setup
			m_ui->slider_step_min->setRange(0, m_steps - 1);
			m_ui->slider_step_max->setRange(0, m_steps - 1);
			m_ui->slider_step_min->setPageStep(m_steps / 20);
			m_ui->slider_step_max->setPageStep(m_steps / 20);
			m_ui->slider_step_max->setValue(m_steps - 1);
			slot_slider();
			//connect
			QObject::connect(m_ui->check_x, SIGNAL(clicked(bool)), this, SLOT(slot_fit(void)));
			QObject::connect(m_ui->check_y, SIGNAL(clicked(bool)), this, SLOT(slot_fit(void)));
			QObject::connect(m_ui->button_fit, SIGNAL(clicked(bool)), this, SLOT(slot_fit(void)));
			QObject::connect(m_ui->tab_x, SIGNAL(currentChanged(int)), this, SLOT(slot_fit(void)));
			QObject::connect(m_ui->tab_y, SIGNAL(currentChanged(int)), this, SLOT(slot_fit(void)));
			QObject::connect(m_ui->button_load, SIGNAL(clicked(bool)), this, SLOT(slot_load(void)));
			QObject::connect(m_ui->button_print, SIGNAL(clicked(bool)), this, SLOT(slot_print(void)));
			QObject::connect(m_ui->button_clean, SIGNAL(clicked(bool)), this, SLOT(slot_clean(void)));
			QObject::connect(m_ui->button_limit, SIGNAL(clicked(void)), this, SLOT(slot_table(void)));
			QObject::connect(m_ui->check_labels, SIGNAL(clicked(void)), this, SLOT(slot_labels(void)));
			QObject::connect(m_ui->check_legend, SIGNAL(clicked(void)), this, SLOT(slot_legend(void)));
			QObject::connect(m_ui->check_points, SIGNAL(stateChanged(int)), this, SLOT(slot_points(void)));
			QObject::connect(m_ui->button_reverse_x, SIGNAL(clicked(bool)), this, SLOT(slot_reverse(void)));
			QObject::connect(m_ui->button_reverse_y, SIGNAL(clicked(bool)), this, SLOT(slot_reverse(void)));
			QObject::connect(m_ui->edit_min_x, SIGNAL(editingFinished(void)), this, SLOT(slot_limits(void)));
			QObject::connect(m_ui->edit_max_x, SIGNAL(editingFinished(void)), this, SLOT(slot_limits(void)));
			QObject::connect(m_ui->edit_min_y, SIGNAL(editingFinished(void)), this, SLOT(slot_limits(void)));
			QObject::connect(m_ui->edit_max_y, SIGNAL(editingFinished(void)), this, SLOT(slot_limits(void)));
			QObject::connect(m_ui->edit_title_1, SIGNAL(editingFinished(void)), this, SLOT(slot_plot(void)));
			QObject::connect(m_ui->edit_title_2, SIGNAL(editingFinished(void)), this, SLOT(slot_plot(void)));
			QObject::connect(m_ui->slider_step_min, SIGNAL(valueChanged(int)), this, SLOT(slot_slider(void)));
			QObject::connect(m_ui->slider_step_max, SIGNAL(valueChanged(int)), this, SLOT(slot_slider(void)));
			//connect combos
			QComboBox* cd[] = {
				m_ui->combo_state_dof_x, m_ui->combo_velocity_dof_x, m_ui->combo_acceleration_dof_x,
				m_ui->combo_state_dof_y, m_ui->combo_velocity_dof_y, m_ui->combo_acceleration_dof_y
			};
			QComboBox* cn[] = {
				m_ui->combo_state_node_x, m_ui->combo_velocity_node_x, m_ui->combo_acceleration_node_x,
				m_ui->combo_state_node_y, m_ui->combo_velocity_node_y, m_ui->combo_acceleration_node_y
			};
			QComboBox* cj[] = {
				m_ui->combo_joint_index_x, m_ui->combo_joint_state_x, m_ui->combo_joint_node_x, 
				m_ui->combo_joint_index_y, m_ui->combo_joint_state_y, m_ui->combo_joint_node_y
			};
			QComboBox* ce[] = {
				m_ui->combo_element_index_x, m_ui->combo_element_state_x, m_ui->combo_element_node_x, 
				m_ui->combo_element_index_y, m_ui->combo_element_state_y, m_ui->combo_element_node_y
			};
			QComboBox* cs[] = {
				m_ui->combo_support_index_x, m_ui->combo_support_index_y
			};
			QObject::connect(cj[1], SIGNAL(activated(int)), this, SLOT(slot_fit(void)));
			QObject::connect(cj[2], SIGNAL(activated(int)), this, SLOT(slot_fit(void)));
			QObject::connect(cj[4], SIGNAL(activated(int)), this, SLOT(slot_fit(void)));
			QObject::connect(cj[5], SIGNAL(activated(int)), this, SLOT(slot_fit(void)));
			QObject::connect(ce[1], SIGNAL(activated(int)), this, SLOT(slot_fit(void)));
			QObject::connect(ce[2], SIGNAL(activated(int)), this, SLOT(slot_fit(void)));
			QObject::connect(ce[4], SIGNAL(activated(int)), this, SLOT(slot_fit(void)));
			QObject::connect(ce[5], SIGNAL(activated(int)), this, SLOT(slot_fit(void)));
			QObject::connect(cj[0], SIGNAL(activated(int)), this, SLOT(slot_joint(void)));
			QObject::connect(cj[3], SIGNAL(activated(int)), this, SLOT(slot_joint(void)));
			QObject::connect(ce[0], SIGNAL(activated(int)), this, SLOT(slot_element(void)));
			QObject::connect(ce[3], SIGNAL(activated(int)), this, SLOT(slot_element(void)));
			QObject::connect(cs[0], SIGNAL(activated(int)), this, SLOT(slot_support(void)));
			QObject::connect(cs[1], SIGNAL(activated(int)), this, SLOT(slot_support(void)));
			for(unsigned i = 0; i < 3; i++)
			{
				QObject::connect(cd[i + 0], SIGNAL(activated(int)), this, SLOT(slot_fit(void)));
				QObject::connect(cd[i + 3], SIGNAL(activated(int)), this, SLOT(slot_fit(void)));
				QObject::connect(cn[i + 0], SIGNAL(activated(int)), this, SLOT(slot_data(void)));
				QObject::connect(cn[i + 3], SIGNAL(activated(int)), this, SLOT(slot_data(void)));
			}
			//set focus
			setFocus(Qt::OtherFocusReason);
			//maximize
			showMaximized();
		}
		
		//destructor
		Equilibrium_Path::~Equilibrium_Path(void)
		{
			const unsigned ns = m_steps;
			const unsigned nn = m_model->mesh()->nodes();
			const unsigned nj = m_model->mesh()->joints();
			const unsigned ne = m_model->mesh()->elements();
			const unsigned nc = m_model->boundary()->supports();
			double*** data[] = {m_state, m_velocity, m_acceleration};
			for(unsigned i = 0; i < 3; i++)
			{
				if(data[i])
				{
					for(unsigned j = 0; j < nn; j++)
					{
						for(unsigned k = 0; k < m_model->mesh()->node(j)->dofs(); k++)
						{
							delete[] data[i][j][k];
						}
						delete[] data[i][j];
					}
					delete[] data[i];
				}
			}
			for(unsigned i = 0; i < nj; i++)
			{
				const unsigned nn = m_model->mesh()->joint(i)->nodes();
				const unsigned ls = unsigned(fea::mesh::joints::state::last);
				const unsigned nr = mat::bit_find(m_model->mesh()->joint(i)->states(), ls);
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
			for(unsigned i = 0; i < ne; i++)
			{
				const unsigned nn = m_model->mesh()->element(i)->nodes();
				const unsigned ls = unsigned(fea::mesh::elements::state::last);
				const unsigned nr = mat::bit_find(m_model->mesh()->element(i)->states(), ls);
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
			for(unsigned i = 0; i < nc; i++)
			{
				delete[] m_support[i];
			}
			delete m_ui;
			delete[] m_step;
			delete[] m_joint;
			delete[] m_element;
			delete[] m_support;
			delete[] m_parameter;
		}
		
		//limits
		void Equilibrium_Path::esc_limit_points(void) const
		{
			//graph
			((QCPCurve*) m_ui->canvas->plottable(2))->data()->clear();
			((QCPCurve*) m_ui->canvas->plottable(2))->removeFromLegend();
			//replot
			m_ui->canvas->replot();
		}
		void Equilibrium_Path::set_limit_points(const std::vector<unsigned>& px, const std::vector<unsigned>& py) const
		{
			//data
			QVector<double> vx, vy;
			const double* dx = current_data(0);
			const double* dy = current_data(0);
			for(unsigned i = 0; i < px.size(); i++)
			{
				vx.push_back(dx[px[i]]);
				vy.push_back(dy[px[i]]);
			}
			for(unsigned i = 0; i < py.size(); i++)
			{
				vx.push_back(dx[py[i]]);
				vy.push_back(dy[py[i]]);
			}
			((QCPCurve*) m_ui->canvas->plottable(2))->setData(vx, vy);
			//replot
			m_ui->canvas->replot();
		}
		
		//events
		void Equilibrium_Path::keyPressEvent(QKeyEvent* event)
		{
			if(event->key() == Qt::Key_Q || event->key() == Qt::Key_Escape)
			{
				close();
			}
			if(event->key() == Qt::Key_F)
			{
				slot_fit();
			}
		}
		
		//data
		void Equilibrium_Path::prepare(void)
		{
			//sizes
			const unsigned nn = m_model->mesh()->nodes();
			const unsigned nj = m_model->mesh()->joints();
			const unsigned ne = m_model->mesh()->elements();
			const unsigned nc = m_model->boundary()->supports();
			//solver
			fea::analysis::solvers::Solver* solver = m_model->analysis()->solver();
			//watch
			const unsigned wn = solver->watch_dof()->m_node;
			const unsigned wt = m_model->mesh()->node(wn)->dof_types();
			const unsigned wd = mat::bit_find(wt, (unsigned) solver->watch_dof()->m_dof);
			//set ui
			m_ui->setupUi(this);
			//combos
			QComboBox* cd[] = {
				m_ui->combo_state_dof_x, m_ui->combo_velocity_dof_x, m_ui->combo_acceleration_dof_x,
				m_ui->combo_state_dof_y, m_ui->combo_velocity_dof_y, m_ui->combo_acceleration_dof_y
			};
			QComboBox* cn[] = {
				m_ui->combo_state_node_x, m_ui->combo_velocity_node_x, m_ui->combo_acceleration_node_x,
				m_ui->combo_state_node_y, m_ui->combo_velocity_node_y, m_ui->combo_acceleration_node_y
			};
			//interface
			char formatter[200];
			const unsigned na[] = {nj, ne, nc};
			const unsigned ss = solver->state_set();
			const std::string path = m_model->path() + "/" + m_model->name() + "/Solver/Load.txt";
			const bool load = boost::filesystem::exists(path);
			m_ui->tab_x->setCurrentIndex(load ? 2 : 1);
			m_ui->tab_y->setCurrentIndex(load ? 1 : 2);
			m_ui->tab_x->setTabText(1, load ? "Load" : "Time");
			m_ui->tab_y->setTabText(1, load ? "Load" : "Time");
			//data
			for(unsigned i = 0; i < 3; i++)
			{
				m_ui->tab_x->setTabEnabled(i + 5, na[i] != 0);
				m_ui->tab_y->setTabEnabled(i + 5, na[i] != 0);
				m_ui->tab_x->setTabEnabled(i + 2, ss & 1 << i);
				m_ui->tab_y->setTabEnabled(i + 2, ss & 1 << i);
				for(unsigned j = 0; j < nn; j++)
				{
					sprintf(formatter, "%04d", j);
					cn[i + 0]->addItem(formatter);
					cn[i + 3]->addItem(formatter);
				}
				for(unsigned j = 1; j < unsigned(fea::mesh::nodes::dof::last); j <<= 1)
				{
					if(j & wt)
					{
						cd[i + 0]->addItem(fea::mesh::nodes::Node::dof_name(fea::mesh::nodes::dof(j)));
						cd[i + 3]->addItem(fea::mesh::nodes::Node::dof_name(fea::mesh::nodes::dof(j)));
					}
				}
				cn[i + 0]->setCurrentIndex(wn);
				cn[i + 3]->setCurrentIndex(wn);
				cd[i + 0]->setCurrentIndex(wd);
				cd[i + 3]->setCurrentIndex(wd);
			}
			//joints
			for(unsigned i = 0; i < nj; i++)
			{
				sprintf(formatter, "%04d", i);
				m_ui->combo_joint_index_x->addItem(formatter);
				m_ui->combo_joint_index_y->addItem(formatter);
			}
			if(nj != 0)
			{
				const unsigned nn = m_model->mesh()->joint(0)->nodes();
				const unsigned st = m_model->mesh()->joint(0)->states();
				for(unsigned i = 0; i < nn; i++)
				{
					sprintf(formatter, "%02d (%04d)", i, m_model->mesh()->joint(0)->index_node(i));
					m_ui->combo_joint_node_x->addItem(formatter);
					m_ui->combo_joint_node_y->addItem(formatter);
				}
				for(unsigned i = 1; i < unsigned(fea::mesh::joints::state::last); i <<= 1)
				{
					if(i & st)
					{
						m_ui->combo_joint_state_x->addItem(fea::mesh::joints::Joint::state_name(fea::mesh::joints::state(i)));
						m_ui->combo_joint_state_y->addItem(fea::mesh::joints::Joint::state_name(fea::mesh::joints::state(i)));
					}
				}
			}
			//elements
			for(unsigned i = 0; i < ne; i++)
			{
				sprintf(formatter, "%04d", i);
				m_ui->combo_element_index_x->addItem(formatter);
				m_ui->combo_element_index_y->addItem(formatter);
			}
			if(ne != 0)
			{
				const unsigned nn = m_model->mesh()->element(0)->nodes();
				const unsigned st = m_model->mesh()->element(0)->states();
				for(unsigned i = 0; i < nn; i++)
				{
					sprintf(formatter, "%02d (%04d)", i, m_model->mesh()->element(0)->index_node(i));
					m_ui->combo_element_node_x->addItem(formatter);
					m_ui->combo_element_node_y->addItem(formatter);
				}
				for(unsigned i = 1; i < unsigned(fea::mesh::elements::state::last); i <<= 1)
				{
					if(i & st)
					{
						m_ui->combo_element_state_x->addItem(fea::mesh::elements::Element::state_name(fea::mesh::elements::state(i)));
						m_ui->combo_element_state_y->addItem(fea::mesh::elements::Element::state_name(fea::mesh::elements::state(i)));
					}
				}
			}
			//supports
			for(unsigned i = 0; i < nc; i++)
			{
				sprintf(formatter, "%04d", i);
				m_ui->combo_support_index_x->addItem(formatter);
				m_ui->combo_support_index_y->addItem(formatter);
			}
			if(nc != 0)
			{
				sprintf(formatter, "%04d", m_model->boundary()->support(0)->index_node());
				const char* s = fea::mesh::nodes::Node::dof_name(m_model->boundary()->support(0)->dof());
				m_ui->label_support_dof_x->setText(s);
				m_ui->label_support_dof_y->setText(s);
				m_ui->label_support_node_x->setText(formatter);
				m_ui->label_support_node_y->setText(formatter);
			}
			//set canvas
			m_ui->canvas->xAxis2->setVisible(true);
			m_ui->canvas->yAxis2->setVisible(true);
			m_ui->canvas->legend->setVisible(true);
			m_ui->canvas->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables| QCP::iSelectAxes | QCP::iSelectLegend);
			//add curve
			new QCPCurve(m_ui->canvas->xAxis, m_ui->canvas->yAxis);
			new QCPCurve(m_ui->canvas->xAxis, m_ui->canvas->yAxis);
			new QCPCurve(m_ui->canvas->xAxis, m_ui->canvas->yAxis);
			//set curve
			m_ui->canvas->plottable(1)->removeFromLegend();
			m_ui->canvas->plottable(2)->removeFromLegend();
			m_ui->canvas->plottable(0)->setName("Galileu");
			m_ui->canvas->plottable(1)->setName("Literature");
			m_ui->canvas->plottable(2)->setName("Limit Points");
			m_ui->canvas->plottable(1)->setPen(QPen(Qt::red));
			m_ui->canvas->plottable(2)->setPen(QPen(Qt::black));
			((QCPCurve*) m_ui->canvas->plottable(1))->setLineStyle(QCPCurve::lsNone);
			((QCPCurve*) m_ui->canvas->plottable(2))->setLineStyle(QCPCurve::lsNone);
			((QCPCurve*) m_ui->canvas->plottable(1))->setScatterStyle(QCPScatterStyle::ssDisc);
			((QCPCurve*) m_ui->canvas->plottable(2))->setScatterStyle(QCPScatterStyle::ssDisc);
			//set ticks
			m_ui->canvas->yAxis->ticker()->setTickCount(10);
			m_ui->canvas->xAxis->ticker()->setTickCount(10);
			m_ui->canvas->yAxis2->ticker()->setTickCount(10);
			m_ui->canvas->xAxis2->ticker()->setTickCount(10);
		}
		void Equilibrium_Path::allocate(void)
		{
			//nodes
			const unsigned nn = m_model->mesh()->nodes();
			const unsigned nj = m_model->mesh()->joints();
			const unsigned ne = m_model->mesh()->elements();
			const unsigned nc = m_model->boundary()->supports();
			const unsigned ss = m_model->analysis()->solver()->state_set();
			//solver
			const std::string path_load = m_model->path() + "/" + m_model->name() + "/Solver/Load.txt";
			const std::string path_time = m_model->path() + "/" + m_model->name() + "/Solver/Time.txt";
			const std::string path_parm = boost::filesystem::exists(path_load) ? path_load : path_time;
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
			//allocate
			m_step = new double[ns];
			m_joint = new double***[nj];
			m_support = new double*[nc];
			m_parameter = new double[ns];
			m_element = new double***[ne];
			double**** data[] = {&m_state, &m_velocity, &m_acceleration};
			for(unsigned i = 0; i < 3; i++)
			{
				if(ss & 1 << i)
				{
					*data[i] = new double**[nn];
					for(unsigned j = 0; j < nn; j++)
					{
						(*data[i])[j] = new double*[m_model->mesh()->node(j)->dofs()];
						for(unsigned k = 0; k < m_model->mesh()->node(j)->dofs(); k++)
						{
							(*data[i])[j][k] = new double[ns];
						}
					}
				}
			}
			for(unsigned i = 0; i < nj; i++)
			{
				const unsigned nn = m_model->mesh()->joint(i)->nodes();
				const unsigned ls = unsigned(fea::mesh::joints::state::last);
				const unsigned nr = mat::bit_find(m_model->mesh()->joint(i)->states(), ls);
				m_joint[i] = new double**[nr];
				for(unsigned j = 0; j < nr; j++)
				{
					m_joint[i][j] = new double*[nn];
					for(unsigned k = 0; k < nn; k++)
					{
						m_joint[i][j][k] = new double[ns];
					}
				}
			}
			for(unsigned i = 0; i < ne; i++)
			{
				const unsigned nn = m_model->mesh()->element(i)->nodes();
				const unsigned ls = unsigned(fea::mesh::elements::state::last);
				const unsigned nr = mat::bit_find(m_model->mesh()->element(i)->states(), ls);
				m_element[i] = new double**[nr];
				for(unsigned j = 0; j < nr; j++)
				{
					m_element[i][j] = new double*[nn];
					for(unsigned k = 0; k < nn; k++)
					{
						m_element[i][j][k] = new double[ns];
					}
				}
			}
			for(unsigned i = 0; i < nc; i++)
			{
				m_support[i] = new double[ns];
			}
		}
		void Equilibrium_Path::get_data(void)
		{
			//sizes
			const unsigned ns = m_steps;
			const unsigned nn = m_model->mesh()->nodes();
			const unsigned nj = m_model->mesh()->joints();
			const unsigned ne = m_model->mesh()->elements();
			const unsigned nc = m_model->boundary()->supports();
			//status
			QStatusBar* status = ((QMainWindow*) parent())->statusBar();
			//solver
			const std::string path_load = m_model->path() + "/" + m_model->name() + "/Solver/Load.txt";
			const std::string path_time = m_model->path() + "/" + m_model->name() + "/Solver/Time.txt";
			const std::string path_parm = boost::filesystem::exists(path_load) ? path_load : path_time;
			//parameter
			FILE* file = fopen(path_parm.c_str(), "r");
			for(unsigned i = 0; i < ns; i++)
			{
				m_step[i] = i;
				fscanf(file, "%lf", &m_parameter[i]);
			}
			fclose(file);
			//interface
			char sj[10], li[10], lj[10];
			sprintf(sj, "%05d", m_steps);
			sprintf(li, "%+.2e", mat::min(m_parameter, ns));
			sprintf(lj, "%+.2e", mat::max(m_parameter, ns));
			m_ui->label_step_max_x->setText(sj);
			m_ui->label_step_max_y->setText(sj);
			m_ui->label_parameter_min_x->setText(li);
			m_ui->label_parameter_min_y->setText(li);
			m_ui->label_parameter_max_x->setText(lj);
			m_ui->label_parameter_max_y->setText(lj);
			//load data
			double*** data[] = {m_state, m_velocity, m_acceleration};
			const char* name[] = {"State", "Velocity", "Acceleration"};
			for(unsigned i = 0; i < 3; i++)
			{
				if(data[i])
				{
					for(unsigned j = 0; j < nn; j++)
					{
						//path
						char path[200], msg[200];
						const std::string folder = m_model->path() + "/" + m_model->name() + "/" + name[i];
						sprintf(path, "%s/N%04d.txt", folder.c_str(), j);
						//file
						FILE* file = fopen(path, "r");
						for(unsigned k = 0; k < ns; k++)
						{
							for(unsigned p = 0; p < m_model->mesh()->node(j)->dofs(); p++)
							{
								fscanf(file, "%lf", &data[i][j][p][k]);
							}
						}
						fclose(file);
						sprintf(msg, "Equilibrium Path: Loading %s (%5.2lf\%)", name[i], 100 * double(j) / nn);
						status->showMessage(msg);
					}
				}
			}
			for(unsigned i = 0; i < nj; i++)
			{
				//sizes
				const unsigned nn = m_model->mesh()->joint(i)->nodes();
				const unsigned ls = unsigned(fea::mesh::joints::state::last);
				const unsigned nr = mat::bit_find(m_model->mesh()->joint(i)->states(), ls);
				//file
				char path[200], msg[200];
				sprintf(path, "%s/%s/Joints/J%04d.txt", m_model->path().c_str(), m_model->name().c_str(), i);
				FILE* file = fopen(path, "r");
				for(unsigned p = 0; p < ns; p++)
				{
					for(unsigned k = 0; k < nn; k++)
					{
						for(unsigned j = 0; j < nr; j++)
						{
							fscanf(file, "%lf", &m_joint[i][j][k][p]);
						}
					}
				}
				fclose(file);
				sprintf(msg, "Equilibrium Path: Loading Joints (%5.2lf\%)", 100 * double(i) / nj);
				status->showMessage(msg);
			}
			for(unsigned i = 0; i < ne; i++)
			{
				//sizes
				const unsigned nn = m_model->mesh()->element(i)->nodes();
				const unsigned ls = unsigned(fea::mesh::elements::state::last);
				const unsigned nr = mat::bit_find(m_model->mesh()->element(i)->states(), ls);
				//file
				char path[200], msg[200];
				sprintf(path, "%s/%s/Elements/E%04d.txt", m_model->path().c_str(), m_model->name().c_str(), i);
				FILE* file = fopen(path, "r");
				for(unsigned p = 0; p < ns; p++)
				{
					for(unsigned k = 0; k < nn; k++)
					{
						for(unsigned j = 0; j < nr; j++)
						{
							fscanf(file, "%lf", &m_element[i][j][k][p]);
						}
					}
				}
				fclose(file);
				sprintf(msg, "Equilibrium Path: Loading Elements (%5.2lf\%)", 100 * double(i) / ne);
				status->showMessage(msg);
			}
			for(unsigned i = 0; i < nc; i++)
			{
				//path
				char path[200], msg[200];
				const std::string folder = m_model->path() + "/" + m_model->name() + "/Supports";
				sprintf(path, "%s/S%04d.txt", folder.c_str(), i);
				//file
				FILE* file = fopen(path, "r");
				for(unsigned j = 0; j < ns; j++)
				{
					fscanf(file, "%lf", &m_support[i][j]);
				}
				fclose(file);
				sprintf(msg, "Equilibrium Path: Loading Supports (%5.2lf\%)", 100 * double(i) / nc);
				status->showMessage(msg);
			}
			status->showMessage("Equilibrium Path: Loading Complete!", 5000);
		}
		
		const QString Equilibrium_Path::current_name(unsigned index) const
		{
			//interface
			const QTabWidget* tab = index == 0 ? m_ui->tab_x : m_ui->tab_y;
			const QLabel* ll[] = {
				index == 0 ? m_ui->label_support_dof_x : m_ui->label_support_dof_y,
				index == 0 ? m_ui->label_support_node_x : m_ui->label_support_node_y
			};
			const QComboBox* cl[] = {
				index == 0 ? m_ui->combo_state_dof_x : m_ui->combo_state_dof_y,
				index == 0 ? m_ui->combo_state_node_x : m_ui->combo_state_node_y,
				index == 0 ? m_ui->combo_velocity_dof_x : m_ui->combo_velocity_dof_y, 
				index == 0 ? m_ui->combo_velocity_node_x : m_ui->combo_velocity_node_y,
				index == 0 ? m_ui->combo_acceleration_dof_x : m_ui->combo_acceleration_dof_y,
				index == 0 ? m_ui->combo_acceleration_node_x : m_ui->combo_acceleration_node_y,
				index == 0 ? m_ui->combo_joint_node_x : m_ui->combo_joint_node_y,
				index == 0 ? m_ui->combo_joint_state_x : m_ui->combo_joint_state_y,
				index == 0 ? m_ui->combo_joint_index_x : m_ui->combo_joint_index_y,
				index == 0 ? m_ui->combo_element_node_x : m_ui->combo_element_node_y,
				index == 0 ? m_ui->combo_element_state_x : m_ui->combo_element_state_y,
				index == 0 ? m_ui->combo_element_index_x : m_ui->combo_element_index_y,
				index == 0 ? m_ui->combo_support_index_x : m_ui->combo_support_index_y
			};
			//name
			char formatter[200];
			switch(tab->currentIndex())
			{
				case 0:
				{
					return "Step";
				}
				case 1:
				{
					return tab->tabText(1);
				}
				case 2:
				{
					const unsigned n = cl[1]->currentIndex();
					const std::string s = cl[0]->currentText().toStdString();
					sprintf(formatter, "State - Node: %04d Dof: %s", n, s.c_str());
					return formatter;
				}
				case 3:
				{
					const unsigned n = cl[2]->currentIndex();
					const std::string s = cl[3]->currentText().toStdString();
					sprintf(formatter, "Velocity - Node: %04d Dof: %s", n, s.c_str());
					return formatter;
				}
				case 4:
				{
					const unsigned n = cl[4]->currentIndex();
					const std::string s = cl[5]->currentText().toStdString();
					sprintf(formatter, "Acceleration - Node: %04d Dof: %s", n, s.c_str());
					return formatter;
				}
				case 5:
				{
					const std::string j = cl[8]->currentText().toStdString();
					const std::string s = cl[7]->currentText().toStdString();
					const std::string n = cl[6]->currentText().toStdString();
					sprintf(formatter, "Joint: %s State: %s Node: %s", j.c_str(), s.c_str(), n.c_str());
					return formatter;
				}
				case 6:
				{
					const std::string e = cl[11]->currentText().toStdString();
					const std::string s = cl[10]->currentText().toStdString();
					const std::string n = cl[ 9]->currentText().toStdString();
					sprintf(formatter, "Element: %s State: %s Node: %s", e.c_str(), s.c_str(), n.c_str());
					return formatter;
				}
				case 7:
				{
					const std::string n = ll[0]->text().toStdString();
					const std::string d = ll[1]->text().toStdString();
					const std::string s = cl[12]->currentText().toStdString();
					sprintf(formatter, "Support: %s - Node: %s Dof: %s", s.c_str(), n.c_str(), d.c_str());
					return formatter;
				}
			}
		}
		const double* Equilibrium_Path::current_data(unsigned index) const
		{
			//interface
			const QTabWidget* tab = index == 0 ? m_ui->tab_x : m_ui->tab_y;
			const QComboBox* cl[] = {
				index == 0 ? m_ui->combo_state_dof_x : m_ui->combo_state_dof_y,
				index == 0 ? m_ui->combo_state_node_x : m_ui->combo_state_node_y,
				index == 0 ? m_ui->combo_velocity_dof_x : m_ui->combo_velocity_dof_y, 
				index == 0 ? m_ui->combo_velocity_node_x : m_ui->combo_velocity_node_y,
				index == 0 ? m_ui->combo_acceleration_dof_x : m_ui->combo_acceleration_dof_y,
				index == 0 ? m_ui->combo_acceleration_node_x : m_ui->combo_acceleration_node_y,
				index == 0 ? m_ui->combo_joint_node_x : m_ui->combo_joint_node_y,
				index == 0 ? m_ui->combo_joint_state_x : m_ui->combo_joint_state_y,
				index == 0 ? m_ui->combo_joint_index_x : m_ui->combo_joint_index_y,
				index == 0 ? m_ui->combo_element_node_x : m_ui->combo_element_node_y,
				index == 0 ? m_ui->combo_element_state_x : m_ui->combo_element_state_y,
				index == 0 ? m_ui->combo_element_index_x : m_ui->combo_element_index_y,
				index == 0 ? m_ui->combo_support_index_x : m_ui->combo_support_index_y
			};
			//data
			switch(tab->currentIndex())
			{
				case 0:
				{
					return m_step;
				}
				case 1:
				{
					return m_parameter;
				}
				case 2:
				{
					const unsigned n = cl[1]->currentIndex();
					const unsigned p = cl[0]->currentIndex();
					return m_state[n][p];
				}
				case 3:
				{
					const unsigned n = cl[3]->currentIndex();
					const unsigned p = cl[2]->currentIndex();
					return m_velocity[n][p];
				}
				case 4:
				{
					const unsigned n = cl[5]->currentIndex();
					const unsigned p = cl[4]->currentIndex();
					return m_acceleration[n][p];
				}
				case 5:
				{
					const unsigned j = cl[8]->currentIndex();
					const unsigned s = cl[7]->currentIndex();
					const unsigned n = cl[6]->currentIndex();
					return m_joint[j][s][n];
				}
				case 6:
				{
					const unsigned e = cl[11]->currentIndex();
					const unsigned s = cl[10]->currentIndex();
					const unsigned n = cl[ 9]->currentIndex();
					return m_element[e][s][n];
				}
				case 7:
				{
					const unsigned s = cl[12]->currentIndex();
					return m_support[s];
				}
			}
		}
		
		//slots
		void Equilibrium_Path::slot_fit(void) const
		{
			const QCheckBox* bl[] = {m_ui->check_x, m_ui->check_y};
			QLineEdit* ll[] = {m_ui->edit_min_x, m_ui->edit_min_y, m_ui->edit_max_x, m_ui->edit_max_y};
			for(unsigned i = 0; i < 2; i++)
			{
				char li[50], lj[50];
				const double* data = current_data(i);
				sprintf(li, "%+.2e", bl[i]->isChecked() ? -mat::max(data, m_steps) : +mat::min(data, m_steps));
				sprintf(lj, "%+.2e", bl[i]->isChecked() ? -mat::min(data, m_steps) : +mat::max(data, m_steps));
				ll[i + 0]->setText(li);
				ll[i + 2]->setText(lj);
			}
			slot_plot();
		}
		void Equilibrium_Path::slot_plot(void) const
		{
			//data
			double* data;
			double dl[2][2];
			QVector<double> vl[2];
			//interface
			QCheckBox* bl[] = {m_ui->check_x, m_ui->check_y};
			const unsigned si = m_ui->slider_step_min->value();
			const unsigned sj = m_ui->slider_step_max->value();
			const QLineEdit* ll[] = {m_ui->edit_min_x, m_ui->edit_min_y, m_ui->edit_max_x, m_ui->edit_max_y};
			//plot
			for(unsigned i = 0; i < 2; i++)
			{
				vl[i].resize(sj - si + 1);
				dl[i][0] = ll[i + 0]->text().toDouble();
				dl[i][1] = ll[i + 2]->text().toDouble();
				mat::set(vl[i].data(), current_data(i) + si, sj - si + 1, bl[i]->isChecked() ? -1 : +1);
			}
			//axis
			m_ui->canvas->xAxis->setRange(dl[0][0], dl[0][1]);
			m_ui->canvas->yAxis->setRange(dl[1][0], dl[1][1]);
			m_ui->canvas->xAxis2->setRange(dl[0][0], dl[0][1]);
			m_ui->canvas->yAxis2->setRange(dl[1][0], dl[1][1]);
			//graph
			((QCPCurve*) m_ui->canvas->plottable(0))->setData(vl[0], vl[1]);
			m_ui->canvas->plottable(0)->setName(m_ui->edit_title_1->text());
			m_ui->canvas->plottable(1)->setName(m_ui->edit_title_2->text());
			//labels
			slot_labels();
		}
		void Equilibrium_Path::slot_load(void) const
		{
			//file
			const unsigned options = 
				(unsigned) QFileDialog::ReadOnly |
				(unsigned) QFileDialog::DontUseNativeDialog | 
				(unsigned) QFileDialog::DontResolveSymlinks;
			const QString path = m_model->path().c_str() + QString("/") + m_model->name().c_str();
			const QString name = QFileDialog::getOpenFileName(nullptr, "Open File", path, "Text (*.txt)", nullptr, (QFileDialog::Options) options);
			if(!name.isEmpty())
			{
				//read
				double a, b;
				QVector<double> x, y;
				FILE* file = fopen(name.toStdString().c_str(), "r");
				while(!feof(file))
				{
					fscanf(file, "%lf", &a);
					fscanf(file, "%lf", &b);
					x.append(a);
					y.append(b);
				}
				fclose(file);
				//graph
				((QCPCurve*) m_ui->canvas->plottable(1))->setData(x, y);
				((QCPCurve*) m_ui->canvas->plottable(1))->addToLegend();
				//plot
				m_ui->canvas->replot();
			}
		}
		void Equilibrium_Path::slot_print(void) const
		{
			//name
			char formatter[500];
			sprintf(formatter, "%s/%s/Equilibrium Path - (%s) x (%s).pdf", 
				m_model->path().c_str(),
				m_model->name().c_str(),
				m_ui->canvas->xAxis->label().toStdString().c_str(), 
				m_ui->canvas->yAxis->label().toStdString().c_str());
			//save
			m_ui->canvas->savePdf(formatter);
			QMessageBox::information(nullptr, "Print", "Printed!", QMessageBox::Ok);
		}
		void Equilibrium_Path::slot_clean(void) const
		{
			//graph
			((QCPCurve*) m_ui->canvas->plottable(1))->data()->clear();
			((QCPCurve*) m_ui->canvas->plottable(2))->data()->clear();
			((QCPCurve*) m_ui->canvas->plottable(1))->removeFromLegend();
			((QCPCurve*) m_ui->canvas->plottable(2))->removeFromLegend();
			//replot
			m_ui->canvas->replot();
		}
		void Equilibrium_Path::slot_points(void) const
		{
			((QCPCurve*) m_ui->canvas->plottable(0))->setScatterStyle(m_ui->check_points->isChecked() ? QCPScatterStyle::ssCircle : QCPScatterStyle::ssNone);
			m_ui->canvas->replot();
		}
		void Equilibrium_Path::slot_limits(void) const
		{
			bool test;
			char formatter[200];
			QLineEdit* list[] = {m_ui->edit_min_x, m_ui->edit_max_x, m_ui->edit_min_y, m_ui->edit_max_y};
			for(unsigned i = 0; i < 4; i++)
			{
				const double v = list[i]->text().toDouble(&test);
				sprintf(formatter, "%+.2e", test ? v : i % 2 ? +1 : -1);
				list[i]->setText(formatter);
			}
			slot_plot();
		}
		void Equilibrium_Path::slot_labels(void) const
		{
			char lb[2][80];
			const bool tl = m_ui->check_labels->isChecked();
			sprintf(lb[0], current_name(0).toStdString().c_str());
			sprintf(lb[1], current_name(1).toStdString().c_str());
			m_ui->canvas->xAxis->setLabel(tl ? lb[0] : "");
			m_ui->canvas->yAxis->setLabel(tl ? lb[1] : "");
			m_ui->canvas->replot();
		}
		void Equilibrium_Path::slot_legend(void) const
		{
			m_ui->canvas->legend->setVisible(m_ui->check_legend->isChecked());
			m_ui->canvas->replot();
		}
		void Equilibrium_Path::slot_slider(void) const
		{
			char formatter[200];
			const unsigned vi = m_ui->slider_step_min->value();
			const unsigned vj = m_ui->slider_step_max->value();
			sprintf(formatter, "Steps: %05d/%05d", std::min(vi, vj), std::max(vi, vj));
			m_ui->label_steps->setText(formatter);
			m_ui->slider_step_min->setValue(std::min(vi, vj));
			m_ui->slider_step_max->setValue(std::max(vi, vj));
			slot_plot();
		}
		
		void Equilibrium_Path::slot_table(void)
		{
			//data
			QVector<double> vxx, vxy;
			QVector<double> vyx, vyy;
			std::vector<unsigned> tx, ty;
			std::vector<unsigned> px, py;
			const double* dx = current_data(0);
			const double* dy = current_data(1);
			for(unsigned i = 1; i + 1 < m_steps; i++)
			{
				const bool txk = dx[i] * dx[i + 1] < 0;
				const bool tyk = dy[i] * dy[i + 1] < 0;
				const bool txi = dx[i] < dx[i - 1] && dx[i] < dx[i + 1];
				const bool txj = dx[i] > dx[i - 1] && dx[i] > dx[i + 1];
				const bool tyi = dy[i] < dy[i - 1] && dy[i] < dy[i + 1];
				const bool tyj = dy[i] > dy[i - 1] && dy[i] > dy[i + 1];
				if(txi || txj || txk)
				{
					px.push_back(i);
					tx.push_back(txi ? 0 : txj ? 1 : 2);
					vxx.push_back(txi || txj ? dx[i] : 0);
					vxy.push_back(txi || txj ? dy[i] : (dy[i] * dx[i + 1] - dx[i] * dy[i + 1]) / (dx[i + 1] - dx[i]));
				}
				if(tyi || tyj || tyk)
				{
					py.push_back(i);
					ty.push_back(tyi ? 0 : tyj ? 1 : 2);
					vyy.push_back(tyi || tyj ? dy[i] : 0);
					vyx.push_back(tyi || tyj ? dx[i] : (dx[i] * dy[i + 1] - dy[i] * dx[i + 1]) / (dy[i + 1] - dy[i]));
				}
			}
			((QCPCurve*) m_ui->canvas->plottable(2))->addToLegend();
			((QCPCurve*) m_ui->canvas->plottable(2))->setData(vxx, vxy);
			((QCPCurve*) m_ui->canvas->plottable(2))->addData(vyx, vyy);
			//replot
			m_ui->canvas->replot();
			//labels
			QString lx = m_ui->canvas->xAxis->label();
			QString ly = m_ui->canvas->yAxis->label();
			//dialog
			const QString l[] = {lx, ly};
			const std::vector<unsigned> t[] = {tx, ty};
			const std::vector<unsigned> p[] = {px, py};
			const QVector<double> v[] = {vxx, vxy, vyx, vyy};
			results::Limits* dia = new results::Limits(v, p, t, l, nullptr);
		    dia->exec();
		    //return
		    delete dia;
		}
		
		void Equilibrium_Path::slot_data(void) const
		{
			//combo
			QComboBox* cd[] = {
				m_ui->combo_state_dof_x, m_ui->combo_velocity_dof_x, m_ui->combo_acceleration_dof_x,
				m_ui->combo_state_dof_y, m_ui->combo_velocity_dof_y, m_ui->combo_acceleration_dof_y
			};
			const QComboBox* cn[] = {
				m_ui->combo_state_node_x, m_ui->combo_velocity_node_x, m_ui->combo_acceleration_node_x,
				m_ui->combo_state_node_y, m_ui->combo_velocity_node_y, m_ui->combo_acceleration_node_y
			};
			//sender
			const unsigned p = std::distance(cn, std::find(cn, cn + 6, (QComboBox*) QObject::sender()));
			//index
			const unsigned a = cd[p]->currentIndex();
			const unsigned b = cn[p]->currentIndex();
			//dofs
			const unsigned n = m_model->mesh()->node(b)->dofs();
			const unsigned q = m_model->mesh()->node(b)->dof_types();
			//fill
			cd[p]->clear();
			for(unsigned i = 1; i < unsigned(fea::mesh::nodes::dof::last); i <<= 1)
			{
				if(i & q)
				{
					cd[p]->addItem(fea::mesh::nodes::Node::dof_name(fea::mesh::nodes::dof(i)));
				}
			}
			//position
			if(a < n)
			{
				cd[p]->setCurrentIndex(a);
			}
			//fit
			slot_fit();
		}
		void Equilibrium_Path::slot_joint(void) const
		{
			//combo
			QComboBox* ce[] = {
				m_ui->combo_joint_index_x, m_ui->combo_joint_state_x, m_ui->combo_joint_node_x, 
				m_ui->combo_joint_index_y, m_ui->combo_joint_state_y, m_ui->combo_joint_node_y
			};
			//sender
			const unsigned p = std::distance(ce, std::find(ce, ce + 6, (QComboBox*) QObject::sender()));
			//index
			const unsigned a = ce[p + 0]->currentIndex();
			const unsigned b = ce[p + 1]->currentIndex();
			const unsigned c = ce[p + 2]->currentIndex();
			const unsigned n = m_model->mesh()->joint(a)->nodes();
			const unsigned s = m_model->mesh()->joint(a)->states();
			//fill
			ce[p + 1]->clear();
			ce[p + 2]->clear();
			char formatter[200];
			for(unsigned i = 0; i < n; i++)
			{
				sprintf(formatter, "%02d (%04d)", i, m_model->mesh()->joint(a)->index_node(i));
				ce[p + 2]->addItem(formatter);
			}
			for(unsigned i = 1; i < unsigned(fea::mesh::joints::state::last); i <<= 1)
			{
				if(i & s)
				{
					ce[p + 1]->addItem(fea::mesh::joints::Joint::state_name(fea::mesh::joints::state(i)));
				}
			}
			//position
			if(c < n)
			{
				ce[p + 2]->setCurrentIndex(c);
			}
			if(b < ce[p + 1]->count())
			{
				ce[p + 1]->setCurrentIndex(b);
			}
			//fit
			slot_fit();		
		}
		void Equilibrium_Path::slot_element(void) const
		{
			//combo
			QComboBox* ce[] = {
				m_ui->combo_element_index_x, m_ui->combo_element_state_x, m_ui->combo_element_node_x, 
				m_ui->combo_element_index_y, m_ui->combo_element_state_y, m_ui->combo_element_node_y
			};
			//sender
			const unsigned p = std::distance(ce, std::find(ce, ce + 6, (QComboBox*) QObject::sender()));
			//index
			const unsigned a = ce[p + 0]->currentIndex();
			const unsigned b = ce[p + 1]->currentIndex();
			const unsigned c = ce[p + 2]->currentIndex();
			const unsigned n = m_model->mesh()->element(a)->nodes();
			const unsigned s = m_model->mesh()->element(a)->states();
			//fill
			ce[p + 1]->clear();
			ce[p + 2]->clear();
			char formatter[200];
			for(unsigned i = 0; i < n; i++)
			{
				sprintf(formatter, "%02d (%04d)", i, m_model->mesh()->element(a)->index_node(i));
				ce[p + 2]->addItem(formatter);
			}
			for(unsigned i = 1; i < unsigned(fea::mesh::elements::state::last); i <<= 1)
			{
				if(i & s)
				{
					ce[p + 1]->addItem(fea::mesh::elements::Element::state_name(fea::mesh::elements::state(i)));
				}
			}
			//position
			if(c < n)
			{
				ce[p + 2]->setCurrentIndex(c);
			}
			if(b < ce[p + 1]->count())
			{
				ce[p + 1]->setCurrentIndex(b);
			}
			//fit
			slot_fit();		
		}
		void Equilibrium_Path::slot_support(void) const
		{
			//combo
			QLabel* ls[] = {
				m_ui->label_support_node_x, m_ui->label_support_dof_x,
				m_ui->label_support_node_y, m_ui->label_support_dof_y
			};
			QComboBox* cs[] = {
				m_ui->combo_support_index_x, m_ui->combo_support_index_y
			};
			//sender
			const unsigned p = std::distance(cs, std::find(cs, cs + 2, (QComboBox*) QObject::sender()));
			//index
			const unsigned s = cs[p]->currentIndex();
			//labels
			char formatter[200];
			sprintf(formatter, "%04d", m_model->boundary()->support(s)->index_node());
			ls[2 * p + 0]->setText(formatter);
			ls[2 * p + 1]->setText(fea::mesh::nodes::Node::dof_name(m_model->boundary()->support(s)->dof()));
			//fit
			slot_fit();
		}
		void Equilibrium_Path::slot_reverse(void) const
		{
			const unsigned i = m_ui->tab_x->currentIndex();
			const unsigned j = m_ui->tab_y->currentIndex();
			m_ui->tab_x->setCurrentIndex(j);
			m_ui->tab_y->setCurrentIndex(i);
		}
	}
}
