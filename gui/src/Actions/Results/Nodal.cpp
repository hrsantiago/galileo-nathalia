//qt
#include <QKeyEvent>
#include <QMessageBox>

//boost
#include <boost/filesystem.hpp>

//mat
#include "misc/util.h"
#include "linear/dense.h"

//fea
#include "Model/Model.h"

#include "Mesh/Mesh.h"
#include "Mesh/Nodes/Dofs.h"
#include "Mesh/Nodes/Node.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Solver.h"

//gui
#include "Actions/Model/Model.h"

#include "Actions/Results/Plots.h"
#include "Actions/Results/Paths.h"
#include "Actions/Results/Nodal.h"

//ui
#include "ui_Nodal.h"
#include "ui_Data_Nodal.h"

namespace gui
{
	namespace results
	{
		//constructors
		Nodal::Nodal(fea::models::Model* model, QWidget* parent) : QDialog(parent), m_play(false), m_ui(new Ui::Nodal), m_model(model)
		{
			//set ui
			m_ui->setupUi(this);
			//set canvas
			m_ui->canvas->update_model(m_model);
			//set slider
			m_ui->slider->setRange(0, m_ui->canvas->m_steps - 1);
			//set combo
			for(unsigned i = 1; i < unsigned(fea::mesh::nodes::dof::last); i <<= 1)
			{
				if(i & m_ui->canvas->m_dofs)
				{
					m_ui->combo_dof->addItem(fea::mesh::nodes::Node::dof_name(fea::mesh::nodes::dof(i)));
				}
			}
			//startup
			slot_slider(0);
			//connect
			QObject::connect(m_ui->button_first, SIGNAL(clicked(bool)), this, SLOT(slot_first(void)));
			QObject::connect(m_ui->button_previous, SIGNAL(clicked(bool)), this, SLOT(slot_previous(void)));
			QObject::connect(m_ui->button_pause, SIGNAL(clicked(bool)), this, SLOT(slot_pause(void)));
			QObject::connect(m_ui->button_play, SIGNAL(clicked(bool)), this, SLOT(slot_play(void)));
			QObject::connect(m_ui->button_stop, SIGNAL(clicked(bool)), this, SLOT(slot_stop(void)));
			QObject::connect(m_ui->button_next, SIGNAL(clicked(bool)), this, SLOT(slot_next(void)));
			QObject::connect(m_ui->button_last, SIGNAL(clicked(bool)), this, SLOT(slot_last(void)));
			QObject::connect(m_ui->button_print, SIGNAL(clicked(bool)), this, SLOT(slot_print(void)));
			QObject::connect(m_ui->button_record, SIGNAL(clicked(bool)), this, SLOT(slot_record(void)));
			QObject::connect(m_ui->button_plot, SIGNAL(clicked(bool)), this, SLOT(slot_plot(void)));
			QObject::connect(m_ui->button_plots, SIGNAL(clicked(bool)), this, SLOT(slot_plots(void)));
			QObject::connect(m_ui->button_paths, SIGNAL(clicked(bool)), this, SLOT(slot_paths(void)));
			QObject::connect(m_ui->edit_skip, SIGNAL(editingFinished(void)), this, SLOT(slot_skip(void)));
			QObject::connect(m_ui->button_skip_add, SIGNAL(clicked(bool)), this, SLOT(slot_skip_add(void)));
			QObject::connect(m_ui->button_skip_sub, SIGNAL(clicked(bool)), this, SLOT(slot_skip_sub(void)));
			QObject::connect(m_ui->slider, SIGNAL(valueChanged(int)), this, SLOT(slot_slider(int)));
			QObject::connect(m_ui->button_min, SIGNAL(clicked(bool)), this, SLOT(slot_push(void)));
			QObject::connect(m_ui->button_max, SIGNAL(clicked(bool)), this, SLOT(slot_push(void)));
			QObject::connect(m_ui->button_limits, SIGNAL(clicked(bool)), this, SLOT(slot_limits(void)));
			//set focus
			setFocus(Qt::OtherFocusReason);
			//maximize
			showMaximized();
		}
		
		//destructor
		Nodal::~Nodal(void)
		{
			delete m_ui;
		}
		
		//step
		unsigned Nodal::dof(void) const
		{
			return (unsigned) m_ui->combo_dof->currentIndex();
		}
		unsigned Nodal::step(void) const
		{
			return (unsigned) m_ui->slider->value();
		}
		
		//events
		void Nodal::keyPressEvent(QKeyEvent* event)
		{
			//key
			const unsigned key = event->key();
			//canvas
			const unsigned ck[] = {
				Qt::Key_X, Qt::Key_Y, Qt::Key_Z, Qt::Key_I, Qt::Key_F, 
				Qt::Key_Right, Qt::Key_Left, Qt::Key_Up, Qt::Key_Down, Qt::Key_Plus, Qt::Key_Minus
			};
			if(std::find(ck, ck + 11, key) != ck + 11)
			{
				m_ui->canvas->keyPressEvent(event);
			}
			//plot
			if(key == Qt::Key_W)
			{
				slot_plot();
			}
			//print
			if(key == Qt::Key_P)
			{
				slot_print();
			}
			//record
			if(key == Qt::Key_R)
			{
				slot_record();
			}
			//close
			if(key == Qt::Key_Q || key == Qt::Key_Escape)
			{
				close();
			}
		}
		
		//slots
		void Nodal::slot_first(void)
		{
			m_play = false;
			m_ui->slider->setValue(0);
		}
		void Nodal::slot_previous(void)
		{
			m_play = false;
			m_ui->slider->setValue(std::max(0, m_ui->slider->value() - 1));
		}
		void Nodal::slot_pause(void)
		{
			m_play = false;
		}
		void Nodal::slot_play(void)
		{
			//play
			m_play = true;
			//skip
			const unsigned sk = m_ui->edit_skip->text().toUInt();
			//max step
			const unsigned sm = m_ui->canvas->m_steps;
			//check slider
			if(m_ui->slider->value() + sk >=  sm)
			{
				m_ui->slider->setValue(0);
			}
			for(unsigned i = m_ui->slider->value(); i < sm; i += sk)
			{
				if(m_play)
				{
					m_ui->slider->setValue(i);
				}
				else
				{
					break;
				}
				QCoreApplication::processEvents(QEventLoop::ProcessEventsFlag::ExcludeSocketNotifiers);
			}
			m_play = false;
		}
		void Nodal::slot_stop(void)
		{
			m_play = false;
			m_ui->slider->setValue(0);
		}
		void Nodal::slot_next(void)
		{
			m_play = false;
			m_ui->slider->setValue(std::min((int) m_ui->canvas->m_steps - 1, m_ui->slider->value() + 1));
		}
		void Nodal::slot_last(void)
		{
			m_play = false;
			m_ui->slider->setValue(m_ui->canvas->m_steps - 1);
		}
		void Nodal::slot_print(void)
		{
			//data
			m_play = false;
			const int step = m_ui->slider->value();
			const QString path = m_model->path().c_str();
			const QString name = m_model->name().c_str();
			const QString file = path + "/" + name + "/Nodal_" + QString::asprintf("%04d", step) + ".png";
			//print
			m_ui->canvas->grabFramebuffer().save(file);
			QMessageBox::information(nullptr, "Nodal", "Nodal image saved!", QMessageBox::Ok);
		}
		void Nodal::slot_record(void)
		{
			m_play = false;
		}
		
		void Nodal::slot_plot(void)
		{
			model::Model* dia = new model::Model(m_model, nullptr, false);
		    dia->exec();
		    delete dia;
		}
		
		void Nodal::slot_plots(void)
		{
			results::Plots* dia = new results::Plots(&m_ui->canvas->m_plots, m_ui->canvas->m_steps - 1, nullptr);
		    dia->exec();
		    delete dia;
		}
		void Nodal::slot_paths(void)
		{
			results::Paths* dia = new results::Paths(m_ui->canvas->m_positions, &m_ui->canvas->m_paths, m_model->mesh()->nodes() - 1, nullptr);
		    dia->exec();
		    delete dia;
		}
		
		void Nodal::slot_skip(void)
		{
			bool t;
			const unsigned ns = m_ui->canvas->m_steps;
			const unsigned nd = m_ui->edit_skip->text().toUInt(&t);
			if(t && nd < ns)
			{
				char formatter[200];
				sprintf(formatter, "%03d", nd);
				m_ui->edit_skip->setText(formatter);
			}
			else
			{
				m_ui->edit_skip->setText("001");
			}
		}
		void Nodal::slot_skip_add(void)
		{
			char formatter[200];
			const unsigned ns = m_ui->canvas->m_steps;
			const unsigned nd = m_ui->edit_skip->text().toUInt() + 1;
			if(nd < ns)
			{
				sprintf(formatter, "%03d", nd);
				m_ui->edit_skip->setText(formatter);
			}
			else
			{
				sprintf(formatter, "%03d", 1);
				m_ui->edit_skip->setText(formatter);
			}
		}
		void Nodal::slot_skip_sub(void)
		{
			char formatter[200];
			const unsigned ns = m_ui->canvas->m_steps;
			const unsigned nd = m_ui->edit_skip->text().toUInt() - 1;
			if(nd > 0)
			{
				sprintf(formatter, "%03d", nd);
				m_ui->edit_skip->setText(formatter);
			}
			else
			{
				sprintf(formatter, "%03d", ns - 1);
				m_ui->edit_skip->setText(formatter);
			}
		}
		
		void Nodal::slot_push(void)
		{
			QPushButton* bl[] = {m_ui->button_min, m_ui->button_max};
			for(unsigned i = 0; i < 2; i++)
			{
				if(bl[i] == QObject::sender())
				{
					m_ui->slider->setValue(m_ui->canvas->m_step[i][m_ui->combo_dof->currentIndex()]);
				}
			}
		}
		void Nodal::slot_slider(int step)
		{
			char formatter[200];
			sprintf(formatter, "Step: %04d/%04d", step, m_ui->canvas->m_steps - 1);
			m_ui->label_step->setText(formatter);
			sprintf(formatter, "%s: %+4.2e", m_model->analysis()->solver()->parameter(), m_ui->canvas->m_parameter[step]);
			m_ui->label_time->setText(formatter);
		}
		void Nodal::slot_limits(void)
		{
			//dialog
			QDialog dia;
			Ui::data_nodal ui;
			char formatter[200];
			ui.setupUi(&dia);
			//solver
			const unsigned ns = m_ui->canvas->m_steps;
			const unsigned index = m_ui->combo_dof->currentIndex();
			sprintf(formatter, "%+4.2e", mat::max(m_ui->canvas->m_parameter, ns));
			ui.label_solver_min_value->setText(formatter);
			sprintf(formatter, "%+4.2e", mat::min(m_ui->canvas->m_parameter, ns));
			ui.label_solver_max_value->setText(formatter);
			//nodes
			sprintf(formatter, "%05d", m_ui->canvas->m_step[0][index]);
			ui.label_data_step_min_value->setText(formatter);
			sprintf(formatter, "%05d", m_ui->canvas->m_step[1][index]);
			ui.label_data_step_max_value->setText(formatter);
			sprintf(formatter, "%05d", m_ui->canvas->m_node[0][index]);
			ui.label_data_node_min_value->setText(formatter);
			sprintf(formatter, "%05d", m_ui->canvas->m_node[1][index]);
			ui.label_data_node_max_value->setText(formatter);
			sprintf(formatter, "%+4.2e", m_ui->canvas->m_limits[0][index]);
			ui.label_data_min_value->setText(formatter);
			sprintf(formatter, "%+4.2e", m_ui->canvas->m_limits[1][index]);
			ui.label_data_max_value->setText(formatter);
			sprintf(formatter, "DOF: %s", m_ui->combo_dof->currentText().toStdString().c_str());
			ui.dof->setTitle(formatter);
			//show
			dia.exec();
		}
	}
}
