//std
#include <cmath>

//qt
#include <QDoubleValidator>

//fea
#include "Model/Model.h"

#include "Mesh/Mesh.h"
#include "Mesh/Sections/Rectangle.h"

//gui
#include "Actions/Mesh/Sections/Rebars.h"
#include "Actions/Mesh/Sections/Rectangle.h"

//ui
#include "ui_Rectangle.h"

namespace gui
{
	namespace mesh
	{
		namespace sections
		{
			//constructors
			Rectangle::Rectangle(fea::mesh::sections::Rectangle* rectangle, QWidget* parent) : QDialog(parent), m_ui(new Ui::Rectangle), m_rectangle(rectangle)
			{
				//set ui
				m_ui->setupUi(this);
				m_ui->canvas->section(m_rectangle);
				setWindowTitle(QString::asprintf("Rectangle - Index: %02d - Name: %s", m_rectangle->index() + 1, m_rectangle->label()));
				//set edit
				m_ui->edit_name->setText(m_rectangle->label());
				m_ui->edit_geom_w->setText(QString::asprintf("%.2e", m_rectangle->width()));
				m_ui->edit_geom_h->setText(QString::asprintf("%.2e", m_rectangle->height()));
				m_ui->edit_mesh_w->setText(QString::asprintf("%02d", m_rectangle->mesh_w()));
				m_ui->edit_mesh_h->setText(QString::asprintf("%02d", m_rectangle->mesh_h()));
				//validators
				m_ui->edit_mesh_w->setValidator(new QIntValidator(0, std::numeric_limits<int>::max()));
				m_ui->edit_mesh_h->setValidator(new QIntValidator(0, std::numeric_limits<int>::max()));
				m_ui->edit_geom_w->setValidator(new QDoubleValidator(0, std::numeric_limits<double>::max(), 3));
				m_ui->edit_geom_h->setValidator(new QDoubleValidator(0, std::numeric_limits<double>::max(), 3));
				//slots
				QObject::connect(m_ui->push_rebars, SIGNAL(clicked(void)), SLOT(slot_rebars(void)));
				QObject::connect(m_ui->edit_name, SIGNAL(editingFinished(void)), SLOT(slot_name(void)));
				QObject::connect(m_ui->edit_geom_w, SIGNAL(editingFinished(void)), SLOT(slot_width(void)));
				QObject::connect(m_ui->edit_geom_h, SIGNAL(editingFinished(void)), SLOT(slot_height(void)));
				QObject::connect(m_ui->edit_mesh_w, SIGNAL(editingFinished(void)), SLOT(slot_mesh_w(void)));
				QObject::connect(m_ui->edit_mesh_h, SIGNAL(editingFinished(void)), SLOT(slot_mesh_h(void)));
			}

			//destructors
			Rectangle::~Rectangle(void)
			{
				delete m_ui;
			}

			//slots
			void Rectangle::slot_name(void)
			{
				m_rectangle->mesh()->model()->mark();
				QString text = m_ui->edit_name->text();
				m_rectangle->label(text.remove(' ').toStdString().c_str());
				m_ui->edit_name->setText(text);
			}
			void Rectangle::slot_width(void)
			{
				//data
				const double w = m_ui->edit_geom_w->text().toDouble();
				//material
				m_rectangle->width(w);
				m_rectangle->mesh()->model()->mark();
				//edit
				m_ui->edit_geom_w->setText(QString::asprintf("%.2e", w));
				//draw
				m_ui->canvas->update();
			}
			void Rectangle::slot_height(void)
			{
				//data
				const double h = m_ui->edit_geom_h->text().toDouble();
				//material
				m_rectangle->height(h);
				m_rectangle->mesh()->model()->mark();
				//edit
				m_ui->edit_geom_h->setText(QString::asprintf("%.2e", h));
				//draw
				m_ui->canvas->update();
			}
			void Rectangle::slot_mesh_w(void)
			{
				//data
				const int w = m_ui->edit_mesh_w->text().toInt();
				//material
				m_rectangle->mesh_w(w);
				m_rectangle->mesh()->model()->mark();
				//edit
				m_ui->edit_mesh_w->setText(QString::asprintf("%02d", w));
				//draw
				m_ui->canvas->update();
			}
			void Rectangle::slot_mesh_h(void)
			{
				//data
				const int h = m_ui->edit_mesh_h->text().toInt();
				//material
				m_rectangle->mesh_h(h);
				m_rectangle->mesh()->model()->mark();
				//edit
				m_ui->edit_mesh_h->setText(QString::asprintf("%02d", h));
				//draw
				m_ui->canvas->update();
			}
			void Rectangle::slot_rebars(void)
			{
				//data
				Rebars(m_rectangle, m_ui->canvas).exec();
				//draw
				m_ui->canvas->update();
			}
		}
	}
}
