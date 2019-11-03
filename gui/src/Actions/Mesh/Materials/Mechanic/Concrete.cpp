//std
#include <cmath>

//qt
#include <QDoubleValidator>

//fea
#include "Model/Model.h"

#include "Mesh/Mesh.h"
#include "Mesh/Materials/Mechanic/Associative/Concrete.h"

//gui
#include "Actions/Mesh/Materials/Mechanic/Concrete.h"

//ui
#include "ui_Concrete.h"

namespace gui
{
	namespace mesh
	{
		namespace materials
		{
			//constructors
			Concrete::Concrete(fea::mesh::materials::Concrete* Concrete, QWidget* parent) : QDialog(parent), m_ui(new Ui::Concrete), m_concrete(Concrete)
			{
				//set ui
				m_ui->setupUi(this);
				setWindowTitle(QString::asprintf("Concrete - Index: %02d - Name: %s", m_concrete->index() + 1, m_concrete->label()));
				//set edits
				m_ui->edit_name->setText(m_concrete->label());
				m_ui->edit_mass->setText(QString::asprintf("%.2e", m_concrete->specific_mass()));
				m_ui->edit_softening->setText(QString::asprintf("%.2e", m_concrete->softening()));
				m_ui->edit_strain->setText(QString::asprintf("%.2e", m_concrete->break_strain()));
				m_ui->edit_elastic->setText(QString::asprintf("%.2e", m_concrete->elastic_modulus()));
				m_ui->edit_tension->setText(QString::asprintf("%.2e", m_concrete->yield_stress_tension()));
				m_ui->edit_biaxial->setText(QString::asprintf("%.2e", m_concrete->yield_stress_biaxial()));
				m_ui->edit_compression->setText(QString::asprintf("%.2e", m_concrete->yield_stress_compression()));
				//validators
				m_ui->edit_mass->setValidator(new QDoubleValidator(0, std::numeric_limits<double>::max(), 3));
				m_ui->edit_strain->setValidator(new QDoubleValidator(0, std::numeric_limits<double>::max(), 3));
				m_ui->edit_elastic->setValidator(new QDoubleValidator(0, std::numeric_limits<double>::max(), 3));
				m_ui->edit_tension->setValidator(new QDoubleValidator(0, std::numeric_limits<double>::max(), 3));
				m_ui->edit_biaxial->setValidator(new QDoubleValidator(0, std::numeric_limits<double>::max(), 3));
				m_ui->edit_softening->setValidator(new QDoubleValidator(0, std::numeric_limits<double>::max(), 3));
				m_ui->edit_compression->setValidator(new QDoubleValidator(0, std::numeric_limits<double>::max(), 3));
				//set canvas
				m_ui->canvas->addGraph();
				m_ui->canvas->addGraph();
				m_ui->canvas->xAxis->setLabel("Strain");
				m_ui->canvas->yAxis->setLabel("Stress");
				m_ui->canvas->axisRect()->setupFullAxesBox();
				m_ui->canvas->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 20)));
				m_ui->canvas->graph(1)->setBrush(QBrush(QColor(0, 0, 255, 20)));
				m_ui->canvas->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
				//slots
				QObject::connect(m_ui->edit_name, SIGNAL(editingFinished(void)), SLOT(slot_name(void)));
				QObject::connect(m_ui->edit_mass, SIGNAL(editingFinished(void)), SLOT(slot_mass(void)));
				QObject::connect(m_ui->edit_strain, SIGNAL(editingFinished(void)), SLOT(slot_strain(void)));
				QObject::connect(m_ui->edit_elastic, SIGNAL(editingFinished(void)), SLOT(slot_elastic(void)));
				QObject::connect(m_ui->edit_tension, SIGNAL(editingFinished(void)), SLOT(slot_tension(void)));
				QObject::connect(m_ui->edit_biaxial, SIGNAL(editingFinished(void)), SLOT(slot_biaxial(void)));
				QObject::connect(m_ui->edit_softening, SIGNAL(editingFinished(void)), SLOT(slot_softening(void)));
				QObject::connect(m_ui->edit_compression, SIGNAL(editingFinished(void)), SLOT(slot_compression(void)));
				QObject::connect(m_ui->canvas->xAxis, SIGNAL(rangeChanged(QCPRange)), m_ui->canvas->xAxis2, SLOT(setRange(QCPRange)));
				QObject::connect(m_ui->canvas->yAxis, SIGNAL(rangeChanged(QCPRange)), m_ui->canvas->yAxis2, SLOT(setRange(QCPRange)));
				//plot
				update_canvas();
			}

			//destructors
			Concrete::~Concrete(void)
			{
				delete m_ui;
			}

			//slots
			void Concrete::slot_name(void)
			{
				m_concrete->mesh()->model()->mark();
				QString text = m_ui->edit_name->text();
				m_concrete->label(text.remove(" ").toStdString().c_str());
				m_ui->edit_name->setText(text);
			}
			void Concrete::slot_mass(void)
			{
				//data
				const double m = m_ui->edit_mass->text().toDouble();
				//material
				m_concrete->specific_mass(m);
				m_concrete->mesh()->model()->mark();
				//edit
				m_ui->edit_mass->setText(QString::asprintf("%.2e", m));
			}
			void Concrete::slot_strain(void)
			{
				//data
				const double e = m_ui->edit_strain->text().toDouble();
				//material
				m_concrete->break_strain(e);
				m_concrete->mesh()->model()->mark();
				//edit
				m_ui->edit_strain->setText(QString::asprintf("%.2e", e));
				//plot
				update_canvas();
			}
			void Concrete::slot_elastic(void)
			{
				//data
				const double E = m_ui->edit_elastic->text().toDouble();
				//material
				m_concrete->elastic_modulus(E);
				m_concrete->mesh()->model()->mark();
				//edit
				m_ui->edit_elastic->setText(QString::asprintf("%.2e", E));
				//plot
				update_canvas();
			}
			void Concrete::slot_tension(void)
			{
				//data
				const double s = m_ui->edit_tension->text().toDouble();
				//material
				m_concrete->yield_stress_tension(s);
				m_concrete->mesh()->model()->mark();
				//edit
				m_ui->edit_tension->setText(QString::asprintf("%.2e", s));
				//plot
				update_canvas();
			}
			void Concrete::slot_biaxial(void)
			{
				//data
				const double s = m_ui->edit_biaxial->text().toDouble();
				//material
				m_concrete->yield_stress_biaxial(s);
				m_concrete->mesh()->model()->mark();
				//edit
				m_ui->edit_biaxial->setText(QString::asprintf("%.2e", s));
				//plot
				update_canvas();
			}
			void Concrete::slot_softening(void)
			{
				//data
				const double u = m_ui->edit_softening->text().toDouble();
				//material
				m_concrete->softening(u);
				m_concrete->mesh()->model()->mark();
				//edit
				m_ui->edit_softening->setText(QString::asprintf("%.2e", u));
				//plot
				update_canvas();
			}
			void Concrete::slot_compression(void)
			{
				//data
				const double s = m_ui->edit_compression->text().toDouble();
				//material
				m_concrete->mesh()->model()->mark();
				m_concrete->yield_stress_compression(s);
				//edit
				m_ui->edit_compression->setText(QString::asprintf("%.2e", s));
				//plot
				update_canvas();
			}
			
			void Concrete::update_canvas(void) const
			{
				//data
				double et = 0, ec = 0;
				const unsigned np = 200;
				QVector<double> x1(np), y1(np);
				QVector<double> x2(np), y2(np);
				const double eu = m_concrete->break_strain();
				//points tension
				for(unsigned i = 0; i < np; i++)
				{
					x1[i] = +eu * i / np;
					y1[i] = return_mapping(x1[i], et);
				}
				//points compression
				for(unsigned i = 0; i < np; i++)
				{
					x2[i] = -eu * i / np;
					y2[i] = return_mapping(x2[i], ec);
				}
				//end points
				y1.append(0);
				y2.append(0);
				x1.append(+eu);
				x2.append(-eu);
				//plot
				m_ui->canvas->graph(0)->setData(x1, y1);
				m_ui->canvas->graph(1)->setData(x2, y2);
				m_ui->canvas->rescaleAxes();
				m_ui->canvas->replot();
			}
			double Concrete::return_mapping(double e, double& ep) const
			{
				//data
				const double n = e > 0 ? +1 : -1;
				const double us = m_concrete->softening();
				const double Et = m_concrete->elastic_modulus();
				const double st = m_concrete->yield_stress_tension();
				const double sb = m_concrete->yield_stress_biaxial();
				const double sc = m_concrete->yield_stress_compression();
				const double A = sc * sb * st * (st + 8 * sb - 3 * sc) / (sc + st) / (2 * sb - sc) / (2 * sb + st);
				const double C = (3 * sb * st - sb * sc - 2 * sc * st) / (sc + st) / (2 * sb - sc) / (2 * sb + st);
				const double B = (st - sc) * (4 * sb * sb + sc * st - sb * (sc + st)) / (sc + st) / (2 * sb - sc) / (2 * sb + st);
				//trial state
				double a = n * ep;
				double s = Et * (e - ep);
				double f = fabs(s) - A * exp(-us * a) - B * s - C * s * s;
				//update
				if(f > 0)
				{
					for(unsigned i = 0; i < 10; i++)
					{
						//corrector
						ep -= f / (Et * (B + 2 * C * s - n) + n * us * A * exp(-us * a));
						//new state
						a = n * ep;
						s = Et * (e - ep);
						f = fabs(s) - A * exp(-us * a) - B * s - C * s * s;
						//check
						if(fabs(f) < 1e-5)
						{
							break;
						}
					}
					
				}
				//return
				return s;
			}
		}
	}
}
