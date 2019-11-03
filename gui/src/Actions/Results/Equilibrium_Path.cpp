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

#include "Actions/Results/Limits.h"
#include "Actions/Results/Equilibrium_Path.h"

//ui
#include "ui_Equilibrium_Path.h"

namespace gui
{
	namespace results
	{
		//constructors
		Equilibrium_Path::Equilibrium_Path(const fea::models::Model* model, const Path* path, QWidget* parent) : QDialog(parent), 
			m_ui(new Ui::Equilibrium_Path), m_model(model), m_path(path)
		{
			//data
			setup();
			slot_fit();
			//setup
			const unsigned ns = m_path->steps();
			m_ui->slider_step_min->setRange(0, ns - 1);
			m_ui->slider_step_max->setRange(0, ns - 1);
			m_ui->slider_step_min->setPageStep(ns / 20);
			m_ui->slider_step_max->setPageStep(ns / 20);
			m_ui->slider_step_max->setValue(ns - 1);
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
			//maximize
			showMaximized();
		}
		
		//destructor
		Equilibrium_Path::~Equilibrium_Path(void)
		{
			delete m_ui;
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
		void Equilibrium_Path::setup(void)
		{
			//sizes
			const unsigned nn = m_model->mesh()->nodes().size();
			const unsigned nj = m_model->mesh()->joints().size();
			const unsigned ne = m_model->mesh()->elements().size();
			const unsigned nc = m_model->boundary()->supports().size();
			//solver
			fea::analysis::solvers::Solver* solver = m_model->analysis()->solver();
			//watch
			const unsigned wn = solver->watch_dof()->m_node;
			const unsigned wt = m_model->mesh()->node(wn)->dof_types();
			const unsigned wd = mat::bit_index(wt, (unsigned) solver->watch_dof()->m_dof);
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
			const unsigned na[] = {nj, ne, nc};
			const unsigned ss = solver->state_set();
			const std::string path = m_model->folder() + "/Solver/Load.txt";
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
					cn[i + 0]->addItem(QString::asprintf("%04d", j));
					cn[i + 3]->addItem(QString::asprintf("%04d", j));
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
				m_ui->combo_joint_index_x->addItem(QString::asprintf("%04d", i));
				m_ui->combo_joint_index_y->addItem(QString::asprintf("%04d", i));
			}
			if(nj != 0)
			{
				const unsigned nn = m_model->mesh()->joint(0)->nodes();
				const unsigned st = m_model->mesh()->joint(0)->states();
				for(unsigned i = 0; i < nn; i++)
				{
					m_ui->combo_joint_node_x->addItem(QString::asprintf("%02d (%04d)", i, m_model->mesh()->joint(0)->index_node(i)));
					m_ui->combo_joint_node_y->addItem(QString::asprintf("%02d (%04d)", i, m_model->mesh()->joint(0)->index_node(i)));
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
				m_ui->combo_element_index_x->addItem(QString::asprintf("%04d", i));
				m_ui->combo_element_index_y->addItem(QString::asprintf("%04d", i));
			}
			if(ne != 0)
			{
				const unsigned st = m_model->mesh()->element(0)->states();
				const unsigned nn = m_model->mesh()->element(0)->nodes().size();
				for(unsigned i = 0; i < nn; i++)
				{
					m_ui->combo_element_node_x->addItem(QString::asprintf("%02d (%04d)", i, m_model->mesh()->element(0)->index_node(i)));
					m_ui->combo_element_node_y->addItem(QString::asprintf("%02d (%04d)", i, m_model->mesh()->element(0)->index_node(i)));
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
				m_ui->combo_support_index_x->addItem(QString::asprintf("%04d", i));
				m_ui->combo_support_index_y->addItem(QString::asprintf("%04d", i));
			}
			if(nc != 0)
			{
				fea::boundary::Support* support = m_model->boundary()->support(0);
				m_ui->label_support_type_x->setText(support->fixed() ? "Fixed" : "Flexible");
				m_ui->label_support_type_y->setText(support->fixed() ? "Fixed" : "Flexible");
				m_ui->label_support_dof_x->setText(fea::mesh::nodes::Node::dof_name(support->dof()));
				m_ui->label_support_dof_y->setText(fea::mesh::nodes::Node::dof_name(support->dof()));
				m_ui->label_support_node_x->setText(QString::asprintf("%04d", support->index_node()));
				m_ui->label_support_node_y->setText(QString::asprintf("%04d", support->index_node()));
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
					return QString::asprintf("State - Node: %04d Dof: %s", n, s.c_str());
				}
				case 3:
				{
					const unsigned n = cl[2]->currentIndex();
					const std::string s = cl[3]->currentText().toStdString();
					return QString::asprintf("Velocity - Node: %04d Dof: %s", n, s.c_str());
				}
				case 4:
				{
					const unsigned n = cl[4]->currentIndex();
					const std::string s = cl[5]->currentText().toStdString();
					return QString::asprintf("Acceleration - Node: %04d Dof: %s", n, s.c_str());
				}
				case 5:
				{
					const std::string j = cl[8]->currentText().toStdString();
					const std::string s = cl[7]->currentText().toStdString();
					const std::string n = cl[6]->currentText().toStdString();
					return QString::asprintf("Joint: %s State: %s Node: %s", j.c_str(), s.c_str(), n.c_str());
				}
				case 6:
				{
					const std::string e = cl[11]->currentText().toStdString();
					const std::string s = cl[10]->currentText().toStdString();
					const std::string n = cl[ 9]->currentText().toStdString();
					return QString::asprintf("Element: %s State: %s Node: %s", e.c_str(), s.c_str(), n.c_str());
				}
				case 7:
				{
					const std::string n = ll[0]->text().toStdString();
					const std::string d = ll[1]->text().toStdString();
					const std::string s = cl[12]->currentText().toStdString();
					return QString::asprintf("Support: %s - Node: %s Dof: %s", s.c_str(), n.c_str(), d.c_str());
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
					return m_path->step();
				}
				case 1:
				{
					return m_path->solver();
				}
				case 2:
				{
					const unsigned n = cl[1]->currentIndex();
					const unsigned p = cl[0]->currentIndex();
					return m_path->node()[0][n][p];
				}
				case 3:
				{
					const unsigned n = cl[3]->currentIndex();
					const unsigned p = cl[2]->currentIndex();
					return m_path->node()[1][n][p];
				}
				case 4:
				{
					const unsigned n = cl[5]->currentIndex();
					const unsigned p = cl[4]->currentIndex();
					return m_path->node()[2][n][p];
				}
				case 5:
				{
					const unsigned j = cl[8]->currentIndex();
					const unsigned s = cl[7]->currentIndex();
					const unsigned n = cl[6]->currentIndex();
					return m_path->joint()[j][s][n];
				}
				case 6:
				{
					const unsigned e = cl[11]->currentIndex();
					const unsigned s = cl[10]->currentIndex();
					const unsigned n = cl[ 9]->currentIndex();
					return m_path->element()[e][s][n];
				}
				case 7:
				{
					const unsigned s = cl[12]->currentIndex();
					return m_path->support()[s];
				}
			}
		}
		
		//slots
		void Equilibrium_Path::slot_fit(void) const
		{
			const unsigned ns = m_path->steps();
			const QCheckBox* bl[] = {m_ui->check_x, m_ui->check_y};
			QLineEdit* ll[] = {m_ui->edit_min_x, m_ui->edit_min_y, m_ui->edit_max_x, m_ui->edit_max_y};
			for(unsigned i = 0; i < 2; i++)
			{
				char li[50], lj[50];
				const double* data = current_data(i);
				sprintf(li, "%+.2e", bl[i]->isChecked() ? -mat::max(data, ns) : +mat::min(data, ns));
				sprintf(lj, "%+.2e", bl[i]->isChecked() ? -mat::min(data, ns) : +mat::max(data, ns));
				ll[i + 0]->setText(li);
				ll[i + 2]->setText(lj);
			}
			slot_plot();
		}
		void Equilibrium_Path::slot_plot(void) const
		{
			//data
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
			const unsigned ns = m_path->steps();
			for(unsigned i = 1; i + 1 < ns; i++)
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
			if(b < (unsigned) ce[p + 1]->count())
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
			const unsigned s = m_model->mesh()->element(a)->states();
			const unsigned n = m_model->mesh()->element(a)->nodes().size();
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
			if(b < (unsigned) ce[p + 1]->count())
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
				m_ui->label_support_node_x, m_ui->label_support_type_x, m_ui->label_support_dof_x,
				m_ui->label_support_node_y, m_ui->label_support_type_y, m_ui->label_support_dof_y
			};
			QComboBox* cs[] = {
				m_ui->combo_support_index_x, m_ui->combo_support_index_y
			};
			//sender
			const unsigned p = std::distance(cs, std::find(cs, cs + 2, (QComboBox*) QObject::sender()));
			//labels
			const unsigned i = cs[p]->currentIndex();
			fea::boundary::Support* support = m_model->boundary()->support(i);
			ls[3 * p + 1]->setText(support->fixed() ? "Fixed" : "Flexible");
			ls[3 * p + 0]->setText(QString::asprintf("%04d", support->index_node()));
			ls[3 * p + 2]->setText(fea::mesh::nodes::Node::dof_name(support->dof()));
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
