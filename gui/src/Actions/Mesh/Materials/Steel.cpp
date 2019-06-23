//std
#include <cmath>

//qt
#include <QDoubleValidator>

//fea
#include "Mesh/Materials/Mechanic/Associative/Steel.h"

//gui
#include "Actions/Mesh/Materials/Steel.h"

//ui
#include "ui_Steel.h"

namespace gui
{
	namespace mesh
	{
		namespace materials
		{
			//constructors
			Steel::Steel(fea::mesh::materials::Steel* Steel, QWidget* parent) : QDialog(parent), m_ui(new Ui::Steel), m_steel(Steel)
			{
				//set ui
				m_ui->setupUi(this);
				setWindowTitle(QString::asprintf("Steel - Index: %02d - Name: %s", m_steel->index(), m_steel->label()));
				//set edits
				m_ui->edit_name->setText(m_steel->label());
				m_ui->edit_mass->setText(QString::asprintf("%.2e", m_steel->specific_mass()));
				m_ui->edit_yield->setText(QString::asprintf("%.2e", m_steel->yield_stress()));
				m_ui->edit_strain->setText(QString::asprintf("%.2e", m_steel->break_strain()));
				m_ui->edit_stress->setText(QString::asprintf("%.2e", m_steel->break_stress()));
				m_ui->edit_poisson->setText(QString::asprintf("%.2e", m_steel->poisson_ratio()));
				m_ui->edit_elastic->setText(QString::asprintf("%.2e", m_steel->elastic_modulus()));
				//validators
				m_ui->edit_mass->setValidator(new QDoubleValidator(0, std::numeric_limits<double>::max(), 3));
				m_ui->edit_yield->setValidator(new QDoubleValidator(0, std::numeric_limits<double>::max(), 3));
				m_ui->edit_strain->setValidator(new QDoubleValidator(0, std::numeric_limits<double>::max(), 3));
				m_ui->edit_stress->setValidator(new QDoubleValidator(0, std::numeric_limits<double>::max(), 3));
				m_ui->edit_poisson->setValidator(new QDoubleValidator(0, std::numeric_limits<double>::max(), 3));
				m_ui->edit_elastic->setValidator(new QDoubleValidator(0, std::numeric_limits<double>::max(), 3));
				//set canvas
				m_ui->canvas->addGraph();
				m_ui->canvas->xAxis->setLabel("Strain");
				m_ui->canvas->yAxis->setLabel("Stress");
				m_ui->canvas->axisRect()->setupFullAxesBox();
				m_ui->canvas->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 20)));
				m_ui->canvas->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
				//slots
				QObject::connect(m_ui->edit_name, SIGNAL(editingFinished(void)), SLOT(slot_name(void)));
				QObject::connect(m_ui->edit_mass, SIGNAL(editingFinished(void)), SLOT(slot_mass(void)));
				QObject::connect(m_ui->edit_yield, SIGNAL(editingFinished(void)), SLOT(slot_yield(void)));
				QObject::connect(m_ui->edit_strain, SIGNAL(editingFinished(void)), SLOT(slot_strain(void)));
				QObject::connect(m_ui->edit_stress, SIGNAL(editingFinished(void)), SLOT(slot_stress(void)));
				QObject::connect(m_ui->edit_poisson, SIGNAL(editingFinished(void)), SLOT(slot_poisson(void)));
				QObject::connect(m_ui->edit_elastic, SIGNAL(editingFinished(void)), SLOT(slot_elastic(void)));
				QObject::connect(m_ui->canvas->xAxis, SIGNAL(rangeChanged(QCPRange)), m_ui->canvas->xAxis2, SLOT(setRange(QCPRange)));
				QObject::connect(m_ui->canvas->yAxis, SIGNAL(rangeChanged(QCPRange)), m_ui->canvas->yAxis2, SLOT(setRange(QCPRange)));
				//plot
				update_canvas();
			}

			//destructors
			Steel::~Steel(void)
			{
				delete m_ui;
			}

			//slots
			void Steel::slot_name(void)
			{
				QString text = m_ui->edit_name->text();
				m_steel->label(text.remove(" ").toStdString().c_str());
				m_ui->edit_name->setText(text);
			}
			void Steel::slot_mass(void)
			{
				//data
				const double m = m_ui->edit_mass->text().toDouble();
				//material
				m_steel->specific_mass(m);
				//edit
				m_ui->edit_mass->setText(QString::asprintf("%.2e", m));
			}
			void Steel::slot_yield(void)
			{
				//data
				const double s = m_ui->edit_yield->text().toDouble();
				//material
				m_steel->yield_stress(s);
				//edit
				m_ui->edit_yield->setText(QString::asprintf("%.2e", s));
				//plot
				update_canvas();
			}
			void Steel::slot_strain(void)
			{
				//data
				const double e = m_ui->edit_strain->text().toDouble();
				//material
				m_steel->break_strain(e);
				//edit
				m_ui->edit_strain->setText(QString::asprintf("%.2e", e));
				//plot
				update_canvas();
			}
			void Steel::slot_stress(void)
			{
				//data
				const double s = m_ui->edit_stress->text().toDouble();
				//material
				m_steel->break_stress(s);
				//edit
				m_ui->edit_stress->setText(QString::asprintf("%.2e", s));
				//plot
				update_canvas();
			}
			void Steel::slot_poisson(void)
			{
				//data
				const double n = m_ui->edit_poisson->text().toDouble();
				//material
				m_steel->poisson_ratio(fmin(n, 0.5));
				//edit
				m_ui->edit_poisson->setText(QString::asprintf("%.2e", fmin(n, 0.5)));
			}
			void Steel::slot_elastic(void)
			{
				//data
				const double E = m_ui->edit_elastic->text().toDouble();
				//material
				m_steel->elastic_modulus(E);
				//edit
				m_ui->edit_elastic->setText(QString::asprintf("%.2e", E));
				//plot
				update_canvas();
			}
			
			//misc
			void Steel::update_canvas(void) const
			{
				//data
				QVector<double> x(6), y(6);
				const double sy = m_steel->yield_stress();
				const double eu = m_steel->break_strain();
				const double su = m_steel->break_stress();
				const double Et = m_steel->elastic_modulus();
				//points
				y[1] = -su;
				y[4] = +su;
				y[2] = -sy;
				y[3] = +sy;
				x[2] = -sy / Et;
				x[3] = +sy / Et;
				y[0] = y[5] = 0;
				x[0] = x[1] = -eu;
				x[4] = x[5] = +eu;
				//plot
				m_ui->canvas->graph(0)->setData(x, y);
				m_ui->canvas->rescaleAxes();
				m_ui->canvas->replot();
			}
		}
	}
}
