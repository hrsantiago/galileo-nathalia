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
#include "Mesh/Elements/States.h"
#include "Mesh/Elements/Element.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Solver.h"

//gui
#include "Limit/State.h"

#include "Results/Path.h"
#include "Results/Canvas.h"
#include "Results/Results.h"

#include "Actions/Model/Model.h"

#include "Actions/Results/Plots.h"
#include "Actions/Results/Paths.h"
#include "Actions/Results/Elements.h"

//ui
#include "ui_State.h"
#include "ui_Elements.h"

namespace gui
{
	namespace results
	{
		//constructors
		Elements::Elements(fea::models::Model* model, const Results* results, QWidget* parent) : QDialog(parent), 
			m_play(false), m_ui(new Ui::Elemental), m_model(model), m_results(results)
		{
			//set ui
			m_ui->setupUi(this);
			//data
			const double* cd = m_results->path()->dof();
			const double* cp = m_results->path()->solver();
			const unsigned ns = m_results->path()->steps();
			const unsigned nn = m_model->mesh()->nodes().size();
			const double*** vp = m_results->canvas()->position();
			const double**** ve = m_results->canvas()->element();
			//set canvas
			m_ui->canvas->element(ve);
			m_ui->canvas->model(m_model);
			m_ui->canvas->position(vp, ns, nn);
			//set slider
			m_ui->slider->setRange(0, ns - 1);
			//set edit
			m_ui->edit_skip->setValidator(new QIntValidator(0, ns - 1));
			m_ui->edit_skip->setText(QString::asprintf("%03d", ns > 100 ? ns / 100 : 1));
			//set combo
			for(unsigned i = 1; i < unsigned(fea::mesh::elements::state::last); i <<= 1)
			{
				if(i & m_results->canvas()->states())
				{
					m_ui->combo_state->addItem(fea::mesh::elements::Element::state_name(fea::mesh::elements::state(i)));
				}
			}
			//connect
			QObject::connect(m_ui->button_min, SIGNAL(clicked(bool)), this, SLOT(slot_push(void)));
			QObject::connect(m_ui->button_max, SIGNAL(clicked(bool)), this, SLOT(slot_push(void)));
			QObject::connect(m_ui->button_play, SIGNAL(clicked(bool)), this, SLOT(slot_play(void)));
			QObject::connect(m_ui->button_stop, SIGNAL(clicked(bool)), this, SLOT(slot_stop(void)));
			QObject::connect(m_ui->button_next, SIGNAL(clicked(bool)), this, SLOT(slot_next(void)));
			QObject::connect(m_ui->button_last, SIGNAL(clicked(bool)), this, SLOT(slot_last(void)));
			QObject::connect(m_ui->button_plot, SIGNAL(clicked(bool)), this, SLOT(slot_plot(void)));
			QObject::connect(m_ui->slider, SIGNAL(valueChanged(int)), this, SLOT(slot_slider(int)));
			QObject::connect(m_ui->button_first, SIGNAL(clicked(bool)), this, SLOT(slot_first(void)));
			QObject::connect(m_ui->button_pause, SIGNAL(clicked(bool)), this, SLOT(slot_pause(void)));
			QObject::connect(m_ui->button_print, SIGNAL(clicked(bool)), this, SLOT(slot_print(void)));
			QObject::connect(m_ui->button_plots, SIGNAL(clicked(bool)), this, SLOT(slot_plots(void)));
			QObject::connect(m_ui->button_paths, SIGNAL(clicked(bool)), this, SLOT(slot_paths(void)));
			QObject::connect(m_ui->button_limits, SIGNAL(clicked(bool)), this, SLOT(slot_limits(void)));
			QObject::connect(m_ui->button_record, SIGNAL(clicked(bool)), this, SLOT(slot_record(void)));
			QObject::connect(m_ui->button_skip_add, SIGNAL(clicked(bool)), this, SLOT(slot_skip_add(void)));
			QObject::connect(m_ui->button_skip_sub, SIGNAL(clicked(bool)), this, SLOT(slot_skip_sub(void)));
			QObject::connect(m_ui->button_previous, SIGNAL(clicked(bool)), this, SLOT(slot_previous(void)));
			QObject::connect(m_ui->combo_state, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_state(void)));
			//maximize
			showMaximized();
		}
		
		//destructor
		Elements::~Elements(void)
		{
			delete m_ui;
		}
		
		//events
		void Elements::keyPressEvent(QKeyEvent* event)
		{
			//event
			const unsigned key = event->key();
			const unsigned mod = event->modifiers();
			//exit
			if(key == Qt::Key_Escape)
			{
				close();
			}
			//plot
			else if(key == Qt::Key_W && mod & Qt::ControlModifier)
			{
				slot_plot();
			}
			//print
			else if(key == Qt::Key_P && mod & Qt::ControlModifier)
			{
				slot_print();
			}
			//canvas
			else
			{
				m_ui->canvas->keyPressEvent(event);
			}
		}
		
		//slots
		void Elements::slot_play(void)
		{
			//play
			m_play = true;
			const unsigned sm = m_results->path()->steps();
			const unsigned sk = m_ui->edit_skip->text().toUInt();
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
		void Elements::slot_stop(void)
		{
			m_play = false;
			m_ui->slider->setValue(0);
		}
		void Elements::slot_next(void)
		{
			m_play = false;
			m_ui->slider->setValue(m_ui->slider->value() + m_ui->edit_skip->text().toUInt());
		}
		void Elements::slot_last(void)
		{
			m_play = false;
			m_ui->slider->setValue(m_results->path()->steps() - 1);
		}
		void Elements::slot_plot(void)
		{
			model::Model(m_model, m_ui->canvas, nullptr, false).exec();
		}
		void Elements::slot_push(void)
		{
			QPushButton* bl[] = {
				m_ui->button_min, 
				m_ui->button_max
			};
			for(unsigned i = 0; i < 2; i++)
			{
				if(bl[i] == QObject::sender())
				{
					const unsigned s = m_ui->combo_state->currentIndex();
					m_ui->slider->setValue(m_results->canvas()->limit_state(s).m_step[i]);
				}
			}
		}
		void Elements::slot_state(void)
		{
			m_ui->canvas->state(m_ui->combo_state->currentIndex());
		}
		void Elements::slot_plots(void)
		{
			results::Plots(m_ui->canvas->m_plots, m_results->path()->steps(), nullptr).exec();
		}
		void Elements::slot_paths(void)
		{
			results::Paths(m_ui->canvas->m_paths, m_model->mesh()->nodes().size(), nullptr).exec();
		}
		void Elements::slot_first(void)
		{
			m_play = false;
			m_ui->slider->setValue(0);
		}
		void Elements::slot_pause(void)
		{
			m_play = false;
		}
		void Elements::slot_print(void)
		{
			//data
			m_play = false;
			const int step = m_ui->slider->value();
			const QString path = m_model->folder().c_str();
			const QString file = path + "/Elements " + QString::asprintf("%04d", step) + ".png";
			//print
			m_ui->canvas->grabFramebuffer().save(file);
			QMessageBox::information(nullptr, "Elements", "Elements image saved!", QMessageBox::Ok);
		}
		void Elements::slot_slider(int step)
		{
			m_ui->canvas->step(step);
			const double* vs = m_results->path()->solver();
			const unsigned ns = m_results->path()->steps();
			m_ui->label_step->setText(QString::asprintf("Step: %04d/%04d", step, ns - 1));
			m_ui->label_solver->setText(QString::asprintf("%s: %+4.2e", m_model->analysis()->solver()->parameter(), vs[step]));
		}
		void Elements::slot_limits(void)
		{
			//dialog
			Ui::State ui;
			QDialog dialog;
			ui.setupUi(&dialog);
			//solver
			const unsigned ns = m_results->path()->steps();
			const unsigned state = m_ui->combo_state->currentIndex();
			ui.label_solver_min->setText(QString::asprintf("%+.2e", mat::max(m_results->path()->solver(), ns)));
			ui.label_solver_max->setText(QString::asprintf("%+.2e", mat::min(m_results->path()->solver(), ns)));
			//state
			ui.state->setTitle("State: " + m_ui->combo_state->currentText());
			ui.label_step_min->setText(QString::asprintf("%04d", m_results->canvas()->limit_state(state).m_step[0]));
			ui.label_step_max->setText(QString::asprintf("%04d", m_results->canvas()->limit_state(state).m_step[1]));
			ui.label_node_min->setText(QString::asprintf("%05d", m_results->canvas()->limit_state(state).m_node[0]));
			ui.label_node_max->setText(QString::asprintf("%05d", m_results->canvas()->limit_state(state).m_node[1]));
			ui.label_value_min->setText(QString::asprintf("%+.2e", m_results->canvas()->limit_state(state).m_value[0]));
			ui.label_value_max->setText(QString::asprintf("%+.2e", m_results->canvas()->limit_state(state).m_value[1]));
			ui.label_element_min->setText(QString::asprintf("%05d", m_results->canvas()->limit_state(state).m_element[0]));
			ui.label_element_max->setText(QString::asprintf("%05d", m_results->canvas()->limit_state(state).m_element[1]));
			//show
			dialog.exec();
		}
		void Elements::slot_record(void)
		{
			m_play = false;
		}
		void Elements::slot_skip_add(void)
		{
			const unsigned ns = m_results->path()->steps();
			const unsigned nd = m_ui->edit_skip->text().toUInt();
			m_ui->edit_skip->setText(QString::asprintf("%03d", nd != ns - 1 ? nd + 1 : ns - 1));
		}
		void Elements::slot_skip_sub(void)
		{
			const unsigned nd = m_ui->edit_skip->text().toUInt();
			m_ui->edit_skip->setText(QString::asprintf("%03d", nd != 1 ? nd - 1 : 1));
		}
		void Elements::slot_previous(void)
		{
			m_play = false;
			m_ui->slider->setValue(m_ui->slider->value() - m_ui->edit_skip->text().toUInt());
		}
	}
}
