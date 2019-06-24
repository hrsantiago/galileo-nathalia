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
#include "Mesh/Nodes/Node.h"
#include "Mesh/Nodes/Dofs.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Solver.h"
#include "Analysis/Solvers/Watch_Dof.h"

//gui
#include "Actions/Model/Model.h"

#include "Actions/Results/Plots.h"
#include "Actions/Results/Paths.h"
#include "Actions/Results/Deformed.h"

//ui
#include "ui_Deformed.h"

namespace gui
{
	namespace results
	{
		//constructors
		Deformed::Deformed(fea::models::Model* model, QWidget* parent) : QDialog(parent), m_play(false), m_ui(new Ui::Deformed), m_model(model)
		{
			//set ui
			m_ui->setupUi(this);
			//set canvas
			m_ui->canvas->update_model(m_model);
			//set slider
			slot_slider(0);
			const unsigned ns = m_ui->canvas->m_steps;
			m_ui->slider->setRange(0, ns - 1);
			//set limits
			char di[50], dj[50], pi[50], pj[50];
			const double* cd = m_ui->canvas->m_dof;
			const double* cp = m_ui->canvas->m_parameter;
			sprintf(di, "Min: %+4.2e", mat::min(cd, ns));
			sprintf(dj, "Max: %+4.2e", mat::max(cd, ns));
			sprintf(pi, "Min: %+4.2e", mat::min(cp, ns));
			sprintf(pj, "Max: %+4.2e", mat::max(cp, ns));
			//set watch
			char ln[50], ld[50];
			const unsigned nw = m_model->analysis()->solver()->watch_dof()->m_node;
			const fea::mesh::nodes::dof dw = m_model->analysis()->solver()->watch_dof()->m_dof;
			sprintf(ln, "Node: %04d", nw);
			sprintf(ld, "Type: %s", fea::mesh::nodes::Node::dof_name(dw));
			//set labels
			m_ui->label_node->setText(ln);
			m_ui->label_type->setText(ld);
			m_ui->label_dof_min->setText(di);
			m_ui->label_dof_max->setText(dj);
			m_ui->label_time_min->setText(pi);
			m_ui->label_time_max->setText(pj);
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
			QObject::connect(m_ui->edit_skip, SIGNAL(editingFinished(void)), this, SLOT(slot_skip(void)));
			QObject::connect(m_ui->button_skip_add, SIGNAL(clicked(bool)), this, SLOT(slot_skip_add(void)));
			QObject::connect(m_ui->button_skip_sub, SIGNAL(clicked(bool)), this, SLOT(slot_skip_sub(void)));
			QObject::connect(m_ui->button_plots, SIGNAL(clicked(bool)), this, SLOT(slot_plots(void)));
			QObject::connect(m_ui->button_paths, SIGNAL(clicked(bool)), this, SLOT(slot_paths(void)));
			QObject::connect(m_ui->slider, SIGNAL(valueChanged(int)), this, SLOT(slot_slider(int)));
			//set focus
			setFocus(Qt::OtherFocusReason);
			//maximize
			showMaximized();
		}
		
		//destructor
		Deformed::~Deformed(void)
		{
			delete m_ui;
		}
		
		//step
		unsigned Deformed::step(void) const
		{
			return (unsigned) m_ui->slider->value();
		}
		
		//events
		void Deformed::keyPressEvent(QKeyEvent* event)
		{
			//key
			const unsigned key = event->key();
			//canvas
			const unsigned ck[] = {
				Qt::Key_X, Qt::Key_Y, Qt::Key_Z, Qt::Key_I, Qt::Key_F, Qt::Key_C, 
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
		void Deformed::slot_first(void)
		{
			m_play = false;
			m_ui->slider->setValue(0);
		}
		void Deformed::slot_previous(void)
		{
			m_play = false;
			m_ui->slider->setValue(std::max(0, m_ui->slider->value() - 1));
		}
		void Deformed::slot_pause(void)
		{
			m_play = false;
		}
		void Deformed::slot_play(void)
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
		void Deformed::slot_stop(void)
		{
			m_play = false;
			m_ui->slider->setValue(0);
		}
		void Deformed::slot_next(void)
		{
			m_play = false;
			m_ui->slider->setValue(std::min((int) m_ui->canvas->m_steps - 1, m_ui->slider->value() + 1));
		}
		void Deformed::slot_last(void)
		{
			m_play = false;
			m_ui->slider->setValue(m_ui->canvas->m_steps - 1);
		}
		void Deformed::slot_print(void)
		{
			//data
			m_play = false;
			const int step = m_ui->slider->value();
			const QString path = m_model->path().c_str();
			const QString name = m_model->name().c_str();
			const QString file = path + "/" + name + "/Deformed_" + QString::asprintf("%04d", step) + ".png";
			//print
			m_ui->canvas->grabFramebuffer().save(file);
			QMessageBox::information(nullptr, "Deformed", "Deformed image saved!", QMessageBox::Ok);
		}
		void Deformed::slot_record(void)
		{
			m_play = false;
		}
		void Deformed::slot_plot(void)
		{
			model::Model* dia = new model::Model(m_model, nullptr, false);
		    dia->exec();
		    delete dia;
		}
		void Deformed::slot_skip(void)
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
		void Deformed::slot_skip_add(void)
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
		void Deformed::slot_skip_sub(void)
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
		void Deformed::slot_plots(void)
		{
			results::Plots* dia = new results::Plots(&m_ui->canvas->m_plots, m_ui->canvas->m_steps - 1, nullptr);
		    dia->exec();
		    delete dia;
		}
		void Deformed::slot_paths(void)
		{
			results::Paths* dia = new results::Paths(m_ui->canvas->m_positions, &m_ui->canvas->m_paths, m_model->mesh()->nodes() - 1, nullptr);
		    dia->exec();
		    delete dia;
		}
		void Deformed::slot_slider(int step)
		{
			char formatter[200];
			sprintf(formatter, "Step: %04d/%04d", step, m_ui->canvas->m_steps - 1);
			m_ui->label_step->setText(formatter);
			sprintf(formatter, "Dof: %+.2e", m_ui->canvas->m_dof[step]);
			m_ui->label_dof->setText(formatter);
			sprintf(formatter, "%s: %+.2e", m_model->analysis()->solver()->parameter(), m_ui->canvas->m_parameter[step]);
			m_ui->label_time->setText(formatter);
		}
	}
}
