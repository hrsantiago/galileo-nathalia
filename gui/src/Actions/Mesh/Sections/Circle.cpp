//std
#include <cmath>

//qt
#include <QDoubleValidator>

//fea
#include "Model/Model.h"

#include "Mesh/Mesh.h"
#include "Mesh/Sections/Circle.h"

//gui
#include "Actions/Mesh/Sections/Circle.h"
#include "Actions/Mesh/Sections/Rebars.h"

//ui
#include "ui_Circle.h"

namespace gui
{
	namespace mesh
	{
		namespace sections
		{
			//constructors
			Circle::Circle(fea::mesh::sections::Circle* circle, QWidget* parent) : QDialog(parent), m_ui(new Ui::Circle), m_circle(circle)
			{
				//set ui
				m_ui->setupUi(this);
				m_ui->canvas->section(m_circle);
				setWindowTitle(QString::asprintf("Circle - Index: %02d - Name: %s", m_circle->index() + 1, m_circle->label()));
				//set edit
				m_ui->edit_name->setText(m_circle->label());
				m_ui->edit_mesh_r->setText(QString::asprintf("%02d", m_circle->mesh_r()));
				m_ui->edit_mesh_t->setText(QString::asprintf("%02d", m_circle->mesh_t()));
				m_ui->edit_geom_d->setText(QString::asprintf("%.2e", m_circle->diameter()));
				//validators
				m_ui->edit_mesh_r->setValidator(new QIntValidator(0, std::numeric_limits<int>::max()));
				m_ui->edit_mesh_t->setValidator(new QIntValidator(0, std::numeric_limits<int>::max()));
				m_ui->edit_geom_d->setValidator(new QDoubleValidator(0, std::numeric_limits<double>::max(), 3));
				//slots
				QObject::connect(m_ui->push_rebars, SIGNAL(clicked(void)), SLOT(slot_rebars(void)));
				QObject::connect(m_ui->edit_name, SIGNAL(editingFinished(void)), SLOT(slot_name(void)));
				QObject::connect(m_ui->edit_mesh_r, SIGNAL(editingFinished(void)), SLOT(slot_mesh_r(void)));
				QObject::connect(m_ui->edit_mesh_t, SIGNAL(editingFinished(void)), SLOT(slot_mesh_t(void)));
				QObject::connect(m_ui->edit_geom_d, SIGNAL(editingFinished(void)), SLOT(slot_diameter(void)));
			}

			//destructors
			Circle::~Circle(void)
			{
				delete m_ui;
			}

			//slots
			void Circle::slot_name(void)
			{
				m_circle->mesh()->model()->mark();
				QString text = m_ui->edit_name->text();
				m_circle->label(text.remove(' ').toStdString().c_str());
				m_ui->edit_name->setText(text);
			}
			void Circle::slot_mesh_r(void)
			{
				//data
				const int r = m_ui->edit_mesh_r->text().toInt();
				//material
				m_circle->mesh_r(r);
				m_circle->mesh()->model()->mark();
				//edit
				m_ui->edit_mesh_r->setText(QString::asprintf("%02d", r));
				//draw
				m_ui->canvas->update();
			}
			void Circle::slot_mesh_t(void)
			{
				//data
				const int t = m_ui->edit_mesh_t->text().toInt();
				//material
				m_circle->mesh_t(t);
				m_circle->mesh()->model()->mark();
				//edit
				m_ui->edit_mesh_t->setText(QString::asprintf("%02d", t));
				//draw
				m_ui->canvas->update();
			}
			void Circle::slot_rebars(void)
			{
				//data
				Rebars(m_circle, m_ui->canvas).exec();
				//draw
				m_ui->canvas->update();
			}
			void Circle::slot_diameter(void)
			{
				//data
				const double d = m_ui->edit_geom_d->text().toDouble();
				//material
				m_circle->diameter(d);
				m_circle->mesh()->model()->mark();
				//edit
				m_ui->edit_geom_d->setText(QString::asprintf("%.2e", d));
				//draw
				m_ui->canvas->update();
			}
		}
	}
}
