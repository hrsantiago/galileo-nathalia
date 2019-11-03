//qt
#include <QKeyEvent>
#include <QMessageBox>
#include <QIntValidator>

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

#include "Results/Path.h"
#include "Results/Canvas.h"
#include "Results/Results.h"

//ui
#include "ui_Deformed.h"

namespace gui
{
	namespace results
	{
		//constructors
		Deformed::Deformed(fea::models::Model* model, const Results* results, QWidget* parent) : QDialog(parent), 
			m_play(false), m_ui(new Ui::Deformed), m_model(model), m_results(results)
		{
			//setup
			m_ui->setupUi(this);
			//data
			const double* cd = m_results->path()->dof();
			const double* cp = m_results->path()->solver();
			const unsigned ns = m_results->path()->steps();
			const unsigned nn = m_model->mesh()->nodes().size();
			const double*** vp = m_results->canvas()->position();
			const unsigned nw = m_model->analysis()->solver()->watch_dof()->m_node;
			const fea::mesh::nodes::dof dw = m_model->analysis()->solver()->watch_dof()->m_dof;
			//set canvas
			m_ui->canvas->model(m_model);
			m_ui->canvas->position(vp, ns, nn);
			//set slider
			m_ui->slider->setRange(0, ns - 1);
			//set edit
			m_ui->edit_skip->setValidator(new QIntValidator(0, ns - 1));
			m_ui->edit_skip->setText(QString::asprintf("%03d", ns > 100 ? ns / 100 : 1));
			//set labels
			m_ui->label_node->setText(QString::asprintf("Node: %04d", nw));
			m_ui->label_dof_min->setText(QString::asprintf("Min: %+.2e", mat::min(cd, ns)));
			m_ui->label_dof_max->setText(QString::asprintf("Max: %+.2e", mat::max(cd, ns)));
			m_ui->label_solver_min->setText(QString::asprintf("Min: %+.2e", mat::min(cp, ns)));
			m_ui->label_solver_max->setText(QString::asprintf("Max: %+.2e", mat::max(cp, ns)));
			m_ui->label_step->setText(QString::asprintf("Step: 0000/%04d", m_results->path()->steps()));
			m_ui->label_type->setText(QString::asprintf("Type: %s", fea::mesh::nodes::Node::dof_name(dw)));
			//connect
			QObject::connect(m_ui->button_plot, SIGNAL(clicked(bool)), this, SLOT(slot_plot(void)));
			QObject::connect(m_ui->button_play, SIGNAL(clicked(bool)), this, SLOT(slot_play(void)));
			QObject::connect(m_ui->button_stop, SIGNAL(clicked(bool)), this, SLOT(slot_stop(void)));
			QObject::connect(m_ui->button_next, SIGNAL(clicked(bool)), this, SLOT(slot_next(void)));
			QObject::connect(m_ui->button_last, SIGNAL(clicked(bool)), this, SLOT(slot_last(void)));
			QObject::connect(m_ui->slider, SIGNAL(valueChanged(int)), this, SLOT(slot_slider(int)));
			QObject::connect(m_ui->button_first, SIGNAL(clicked(bool)), this, SLOT(slot_first(void)));
			QObject::connect(m_ui->button_pause, SIGNAL(clicked(bool)), this, SLOT(slot_pause(void)));
			QObject::connect(m_ui->button_plots, SIGNAL(clicked(bool)), this, SLOT(slot_plots(void)));
			QObject::connect(m_ui->button_print, SIGNAL(clicked(bool)), this, SLOT(slot_print(void)));
			QObject::connect(m_ui->button_paths, SIGNAL(clicked(bool)), this, SLOT(slot_paths(void)));
			QObject::connect(m_ui->button_record, SIGNAL(clicked(bool)), this, SLOT(slot_record(void)));
			QObject::connect(m_ui->button_skip_add, SIGNAL(clicked(bool)), this, SLOT(slot_skip_add(void)));
			QObject::connect(m_ui->button_skip_sub, SIGNAL(clicked(bool)), this, SLOT(slot_skip_sub(void)));
			QObject::connect(m_ui->button_previous, SIGNAL(clicked(bool)), this, SLOT(slot_previous(void)));
			//maximize
			showMaximized();
		}
		
		//destructor
		Deformed::~Deformed(void)
		{
			delete m_ui;
		}
		
		//events
		void Deformed::keyPressEvent(QKeyEvent* event)
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
		void Deformed::slot_play(void)
		{
			//data
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
		void Deformed::slot_stop(void)
		{
			m_play = false;
			m_ui->slider->setValue(0);
		}
		void Deformed::slot_next(void)
		{
			m_play = false;
			m_ui->slider->setValue(m_ui->slider->value() + m_ui->edit_skip->text().toUInt());
		}
		void Deformed::slot_last(void)
		{
			m_play = false;
			m_ui->slider->setValue(m_results->path()->steps() - 1);
		}
		void Deformed::slot_plot(void)
		{
			model::Model(m_model, m_ui->canvas, nullptr, false).exec();
		}
		void Deformed::slot_first(void)
		{
			m_play = false;
			m_ui->slider->setValue(0);
		}
		void Deformed::slot_pause(void)
		{
			m_play = false;
		}
		void Deformed::slot_plots(void)
		{
			results::Plots(m_ui->canvas->m_plots, m_results->path()->steps(), nullptr).exec();
			m_ui->canvas->redraw();
		}
		void Deformed::slot_paths(void)
		{
			results::Paths(m_ui->canvas->m_paths, m_model->mesh()->nodes().size(), nullptr).exec();
			m_ui->canvas->redraw();
		}
		void Deformed::slot_print(void)
		{
			//data
			m_play = false;
			const int step = m_ui->slider->value();
			const QString path = m_model->folder().c_str();
			const QString file = path + "/Deformed " + QString::asprintf("%05d", step) + ".png";
			//print
			m_ui->canvas->grabFramebuffer().save(file);
			QMessageBox::information(nullptr, "Deformed", "Deformed image saved!", QMessageBox::Ok);
		}
		void Deformed::slot_slider(int step)
		{
			m_ui->canvas->step(step);
			const double* vd = m_results->path()->dof();
			const double* vs = m_results->path()->solver();
			const unsigned ns = m_results->path()->steps();
			m_ui->label_dof->setText(QString::asprintf("Dof: %+.2e", vd[step]));
			m_ui->label_step->setText(QString::asprintf("Step: %04d/%04d", step, ns - 1));
			m_ui->label_solver->setText(QString::asprintf("%s: %+.2e", m_model->analysis()->solver()->parameter(), vs[step]));
		}
		void Deformed::slot_record(void)
		{
			m_play = false;
		}
		void Deformed::slot_skip_add(void)
		{
			const unsigned ns = m_results->path()->steps();
			const unsigned nd = m_ui->edit_skip->text().toUInt();
			m_ui->edit_skip->setText(QString::asprintf("%03d", nd != ns - 1 ? nd + 1 : ns - 1));
		}
		void Deformed::slot_skip_sub(void)
		{
			const unsigned nd = m_ui->edit_skip->text().toUInt();
			m_ui->edit_skip->setText(QString::asprintf("%03d", nd != 0 ? nd - 1 : 0));
		}
		void Deformed::slot_previous(void)
		{
			m_play = false;
			m_ui->slider->setValue(m_ui->slider->value() - m_ui->edit_skip->text().toUInt());
		}
	}
}
