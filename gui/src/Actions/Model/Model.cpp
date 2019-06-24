//std
#include <functional>

//qt
#include <QThread>
#include <QKeyEvent>
#include <QColorDialog>

//fea
#include "Model/Model.h"

#include "Plot/Plot.h"
#include "Plot/What.h"
#include "Plot/Sizes.h"
#include "Plot/Colors.h"
#include "Plot/Gradient.h"

#include "Boundary/Boundary.h"
#include "Boundary/Cases/Load_Case.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Solver.h"

//gui
#include "Actions/Model/Model.h"

//ui
#include "ui_Model.h"

namespace gui
{
	namespace model
	{
		//constructors
		Model::Model(fea::models::Model* model, QWidget* parent, bool edit) : QDialog(parent), m_ui(new Ui::Model), m_model(model)
		{
			//set ui
			m_ui->setupUi(this);
			setFixedSize(width(), height());
			m_ui->edit_name->setEnabled(edit);
			m_ui->edit_name->setText(model->name().c_str());
			//set what
			m_ui->check_axes->setCheckState(m_model->plot()->what()->axes() ? Qt::Checked : Qt::Unchecked);
			m_ui->check_scale->setCheckState(m_model->plot()->what()->scale() ? Qt::Checked : Qt::Unchecked);
			m_ui->check_nodes->setCheckState(m_model->plot()->what()->nodes() ? Qt::Checked : Qt::Unchecked);
			m_ui->check_loads->setCheckState(m_model->plot()->what()->loads() ? Qt::Checked : Qt::Unchecked);
			m_ui->check_joints->setCheckState(m_model->plot()->what()->joints() ? Qt::Checked : Qt::Unchecked);
			m_ui->check_elements->setCheckState(m_model->plot()->what()->elements() ? Qt::Checked : Qt::Unchecked);
			m_ui->check_supports->setCheckState(m_model->plot()->what()->supports() ? Qt::Checked : Qt::Unchecked);
			//set sizes
			char w[200];
			const fea::plot::Sizes* p = m_model->plot()->sizes();
			const double s[] = {p->loads(), p->supports(), p->joints()};
			QLineEdit* l[] = {m_ui->edit_size_loads, m_ui->edit_size_supports, m_ui->edit_size_joints};
			for(unsigned i = 0; i < 3; i++)
			{
				sprintf(w, "%.2lf", 100 * s[i]);
				l[i]->setText(w);
			}
			//load cases
			if(m_model->boundary()->load_cases() == 0)
			{
				m_ui->combo_case->setEnabled(false);
			}
			else
			{
				for(unsigned i = 0; i < m_model->boundary()->load_cases(); i++)
				{
					m_ui->combo_case->addItem(m_model->boundary()->load_case(i)->label());
				}
				m_ui->combo_case->setCurrentIndex(m_model->analysis()->solver()->load_case());
			}
			//connect
			QObject::connect(m_ui->edit_name, SIGNAL(textChanged(const QString&)), this, SLOT(slot_edit(void)));
			QObject::connect(m_ui->check_axes, SIGNAL(stateChanged(int)), this, SLOT(slot_check(void)));
			QObject::connect(m_ui->check_scale, SIGNAL(stateChanged(int)), this, SLOT(slot_check(void)));
			QObject::connect(m_ui->check_nodes, SIGNAL(stateChanged(int)), this, SLOT(slot_check(void)));
			QObject::connect(m_ui->check_loads, SIGNAL(stateChanged(int)), this, SLOT(slot_check(void)));
			QObject::connect(m_ui->check_joints, SIGNAL(stateChanged(int)), this, SLOT(slot_check(void)));
			QObject::connect(m_ui->check_elements, SIGNAL(stateChanged(int)), this, SLOT(slot_check(void)));
			QObject::connect(m_ui->check_supports, SIGNAL(stateChanged(int)), this, SLOT(slot_check(void)));
			QObject::connect(m_ui->button_back, SIGNAL(clicked(bool)), this, SLOT(slot_push(void)));
			QObject::connect(m_ui->button_paths, SIGNAL(clicked(bool)), this, SLOT(slot_push(void)));
			QObject::connect(m_ui->button_nodes, SIGNAL(clicked(bool)), this, SLOT(slot_push(void)));
			QObject::connect(m_ui->button_loads, SIGNAL(clicked(bool)), this, SLOT(slot_push(void)));
			QObject::connect(m_ui->button_joints, SIGNAL(clicked(bool)), this, SLOT(slot_push(void)));
			QObject::connect(m_ui->button_elements, SIGNAL(clicked(bool)), this, SLOT(slot_push(void)));
			QObject::connect(m_ui->button_supports, SIGNAL(clicked(bool)), this, SLOT(slot_push(void)));
			QObject::connect(m_ui->button_scale_min, SIGNAL(clicked(bool)), this, SLOT(slot_grad(void)));
			QObject::connect(m_ui->button_scale_max, SIGNAL(clicked(bool)), this, SLOT(slot_grad(void)));
			QObject::connect(m_ui->button_scale_mean, SIGNAL(clicked(bool)), this, SLOT(slot_grad(void)));
			QObject::connect(m_ui->combo_case, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_combo(void)));
			QObject::connect(m_ui->button_reset, SIGNAL(clicked(bool)), this, SLOT(slot_reset(void)));
			QObject::connect(m_ui->button_print, SIGNAL(clicked(bool)), this, SLOT(slot_print(void)));
			QObject::connect(m_ui->edit_size_loads, SIGNAL(editingFinished(void)), this, SLOT(slot_sizes(void)));
			QObject::connect(m_ui->edit_size_joints, SIGNAL(editingFinished(void)), this, SLOT(slot_sizes(void)));
			QObject::connect(m_ui->edit_size_supports, SIGNAL(editingFinished(void)), this, SLOT(slot_sizes(void)));
		}
		
		//destructor
		Model::~Model(void)
		{
			delete m_ui;
		}
		
		//slots
		void Model::slot_edit(void)
		{
			m_model->mark();
			m_model->name(m_ui->edit_name->text().toStdString());
		}
		void Model::slot_push(void)
		{
			const char* names[] = {
				"Back", 
				"Nodes", 
				"Loads", 
				"Paths", 
				"Joints", 
				"Elements",
				"Supports" 
			};
			const QPushButton* b[] = {
				m_ui->button_back, 
				m_ui->button_nodes, 
				m_ui->button_loads, 
				m_ui->button_paths,
				m_ui->button_joints, 
				m_ui->button_elements, 
				m_ui->button_supports
			};
			const std::function<const double*(void)> g[] = {
				[this] (void) { return m_model->plot()->colors()->back(); },
				[this] (void) { return m_model->plot()->colors()->nodes(); },
				[this] (void) { return m_model->plot()->colors()->loads(); },
				[this] (void) { return m_model->plot()->colors()->paths(); },
				[this] (void) { return m_model->plot()->colors()->joints(); },
				[this] (void) { return m_model->plot()->colors()->elements(); },
				[this] (void) { return m_model->plot()->colors()->supports(); }
			};
			const std::function<void(QColor)> f[] = {
				[this] (QColor c) { double q[] = {c.redF(), c.greenF(), c.blueF(), c.alphaF()}; m_model->plot()->colors()->back(q); },
				[this] (QColor c) { double q[] = {c.redF(), c.greenF(), c.blueF(), c.alphaF()}; m_model->plot()->colors()->nodes(q); },
				[this] (QColor c) { double q[] = {c.redF(), c.greenF(), c.blueF(), c.alphaF()}; m_model->plot()->colors()->loads(q); },
				[this] (QColor c) { double q[] = {c.redF(), c.greenF(), c.blueF(), c.alphaF()}; m_model->plot()->colors()->paths(q); },
				[this] (QColor c) { double q[] = {c.redF(), c.greenF(), c.blueF(), c.alphaF()}; m_model->plot()->colors()->joints(q); },
				[this] (QColor c) { double q[] = {c.redF(), c.greenF(), c.blueF(), c.alphaF()}; m_model->plot()->colors()->elements(q); },
				[this] (QColor c) { double q[] = {c.redF(), c.greenF(), c.blueF(), c.alphaF()}; m_model->plot()->colors()->supports(q); }
			};
			for(unsigned i = 0; i < 7; i++)
			{
				if(b[i] == QObject::sender())
				{
					char title[200];
					const double* c = g[i]();
					const int r = int(255 * c[0]);
					const int g = int(255 * c[1]);
					const int b = int(255 * c[2]);
					const int a = int(255 * c[3]);
					QColor start = QColor(r, g, b, a);
					sprintf(title, "%s Color", names[i]);
					QColor color = QColorDialog::getColor(start, this, title, QColorDialog::ShowAlphaChannel);
					if(color.isValid())
					{
						f[i](color);
					}
					break;
				}
			}
		}
		void Model::slot_grad(void)
		{
			const char* names[] = {
				"Min", 
				"Max",
				"Mean"
			};
			const QPushButton* bl[] = {
				m_ui->button_scale_min, 
				m_ui->button_scale_max,
				m_ui->button_scale_mean 
			};
			const std::function<const double*(void)> g[] = {
				[this] (void) { return m_model->plot()->gradient()->color_min(); },
				[this] (void) { return m_model->plot()->gradient()->color_max(); },
				[this] (void) { return m_model->plot()->gradient()->color_mean(); }
			};
			const std::function<void(QColor)> f[] = {
				[this] (QColor c) { double q[] = {c.redF(), c.greenF(), c.blueF(), c.alphaF()}; m_model->plot()->gradient()->color_min(q); },
				[this] (QColor c) { double q[] = {c.redF(), c.greenF(), c.blueF(), c.alphaF()}; m_model->plot()->gradient()->color_max(q); },
				[this] (QColor c) { double q[] = {c.redF(), c.greenF(), c.blueF(), c.alphaF()}; m_model->plot()->gradient()->color_mean(q); }
			};
			for(unsigned i = 0; i < 3; i++)
			{
				if(bl[i] == QObject::sender())
				{
					char title[200];
					const double* c = g[i]();
					const int r = int(255 * c[0]);
					const int g = int(255 * c[1]);
					const int b = int(255 * c[2]);
					const int a = int(255 * c[3]);
					QColor start = QColor(r, g, b, a);
					sprintf(title, "%s Color", names[i]);
					QColor color = QColorDialog::getColor(start, this, title, QColorDialog::ShowAlphaChannel);
					if(color.isValid())
					{
						f[i](color);
					}
					break;
				}
			}
		}
		void Model::slot_sizes(void)
		{
			char w[200];
			QLineEdit* ll[] = {
				m_ui->edit_size_loads, 
				m_ui->edit_size_joints, 
				m_ui->edit_size_supports
			};
			const std::function<void(double)> f[] = {
				[this] (double v) { m_model->plot()->sizes()->loads(v); },
				[this] (double v) { m_model->plot()->sizes()->joints(v); },
				[this] (double v) { m_model->plot()->sizes()->supports(v); }
			};
			for(unsigned i = 0; i < 3; i++)
			{	
				if(ll[i] == QObject::sender())
				{
					f[i](ll[i]->text().toDouble() / 100);
					sprintf(w, "%.2lf", ll[i]->text().toDouble());
					ll[i]->setText(w);
					break;
				}
			}
		}
		void Model::slot_check(void)
		{
			const QCheckBox* c[] = {
				m_ui->check_axes, 
				m_ui->check_scale, 
				m_ui->check_nodes, 
				m_ui->check_loads, 
				m_ui->check_joints, 
				m_ui->check_elements, 
				m_ui->check_supports
			};
			const std::function<void(bool)> f[] = {
				[this] (bool t) { m_model->plot()->what()->axes(t); },
				[this] (bool t) { m_model->plot()->what()->scale(t); },
				[this] (bool t) { m_model->plot()->what()->nodes(t); },
				[this] (bool t) { m_model->plot()->what()->loads(t); },
				[this] (bool t) { m_model->plot()->what()->joints(t); },
				[this] (bool t) { m_model->plot()->what()->elements(t); },
				[this] (bool t) { m_model->plot()->what()->supports(t); }
			};
			for(unsigned i = 0; i < 7; i++)
			{
				if(c[i] == QObject::sender())
				{
					f[i](c[i]->isChecked());
					break;
				}
			}
		}
		void Model::slot_combo(void)
		{
			m_model->analysis()->solver()->load_case(m_ui->combo_case->currentIndex());
		}
		void Model::slot_reset(void)
		{
			//reset
			m_model->plot()->reset();
			//set what
			m_ui->check_axes->setCheckState(m_model->plot()->what()->axes() ? Qt::Checked : Qt::Unchecked);
			m_ui->check_scale->setCheckState(m_model->plot()->what()->scale() ? Qt::Checked : Qt::Unchecked);
			m_ui->check_nodes->setCheckState(m_model->plot()->what()->nodes() ? Qt::Checked : Qt::Unchecked);
			m_ui->check_loads->setCheckState(m_model->plot()->what()->loads() ? Qt::Checked : Qt::Unchecked);
			m_ui->check_joints->setCheckState(m_model->plot()->what()->joints() ? Qt::Checked : Qt::Unchecked);
			m_ui->check_elements->setCheckState(m_model->plot()->what()->elements() ? Qt::Checked : Qt::Unchecked);
			m_ui->check_supports->setCheckState(m_model->plot()->what()->supports() ? Qt::Checked : Qt::Unchecked);
			//set sizes
			char w[200];
			const fea::plot::Sizes* p = m_model->plot()->sizes();
			const double s[] = {p->loads(), p->supports(), p->joints()};
			QLineEdit* l[] = {m_ui->edit_size_loads, m_ui->edit_size_supports, m_ui->edit_size_joints};
			for(unsigned i = 0; i < 3; i++)
			{
				sprintf(w, "%.2lf", 100 * s[i]);
				l[i]->setText(w);
			}
		}
		void Model::slot_print(void)
		{
			m_model->plot()->colors()->print();
		}
	}
}
