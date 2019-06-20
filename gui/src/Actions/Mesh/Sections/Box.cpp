//std
#include <cmath>

//fea
#include "Mesh/Mesh.h"
#include "Mesh/Sections/Box.h"

//gui
#include "Actions/Mesh/Sections/Box.h"
#include "Actions/Mesh/Sections/Rebars.h"

//ui
#include "uic/Mesh/Sections/Box.h"

namespace gui
{
	namespace mesh
	{
		namespace sections
		{
			//constructors
			Box::Box(fea::mesh::sections::Box* box, QWidget* parent) : QDialog(parent), m_ui(new Ui::Box), m_box(box)
			{
				//set ui
				m_ui->setupUi(this);
				m_ui->canvas->section(m_box);
				setWindowTitle(QString::asprintf("Box - Index: %02d - Name: %s", m_box->index(), m_box->label()));
				//set edit
				m_ui->edit_name->setText(m_box->label());
				m_ui->edit_geom_w->setText(QString::asprintf("%.2e", m_box->width()));
				m_ui->edit_geom_h->setText(QString::asprintf("%.2e", m_box->height()));
				m_ui->edit_mesh_w->setText(QString::asprintf("%02d", m_box->mesh_w()));
				m_ui->edit_mesh_h->setText(QString::asprintf("%02d", m_box->mesh_h()));
				m_ui->edit_mesh_t->setText(QString::asprintf("%02d", m_box->mesh_t()));
				m_ui->edit_geom_t->setText(QString::asprintf("%.2e", m_box->thickness()));
				//validators
				m_ui->edit_mesh_w->setValidator(new QIntValidator(0, std::numeric_limits<int>::max()));
				m_ui->edit_mesh_h->setValidator(new QIntValidator(0, std::numeric_limits<int>::max()));
				m_ui->edit_geom_w->setValidator(new QDoubleValidator(0, std::numeric_limits<double>::max(), 3));
				m_ui->edit_geom_t->setValidator(new QDoubleValidator(0, std::numeric_limits<double>::max(), 3));
				m_ui->edit_geom_h->setValidator(new QDoubleValidator(0, std::numeric_limits<double>::max(), 3));
				//slots
				QObject::connect(m_ui->push_rebars, SIGNAL(clicked(void)), SLOT(slot_rebars(void)));
				QObject::connect(m_ui->edit_name, SIGNAL(editingFinished(void)), SLOT(slot_name(void)));
				QObject::connect(m_ui->edit_geom_w, SIGNAL(editingFinished(void)), SLOT(slot_width(void)));
				QObject::connect(m_ui->edit_geom_t, SIGNAL(editingFinished(void)), SLOT(slot_thick(void)));
				QObject::connect(m_ui->edit_geom_h, SIGNAL(editingFinished(void)), SLOT(slot_height(void)));
				QObject::connect(m_ui->edit_mesh_w, SIGNAL(editingFinished(void)), SLOT(slot_mesh_w(void)));
				QObject::connect(m_ui->edit_mesh_h, SIGNAL(editingFinished(void)), SLOT(slot_mesh_h(void)));
				QObject::connect(m_ui->edit_mesh_t, SIGNAL(editingFinished(void)), SLOT(slot_mesh_t(void)));
			}

			//destructors
			Box::~Box(void)
			{
				delete m_ui;
			}

			//slots
			void Box::slot_name(void)
			{
				QString text = m_ui->edit_name->text();
				m_box->label(text.remove(' ').toStdString().c_str());
				m_ui->edit_name->setText(text);
			}
			void Box::slot_width(void)
			{
				//data
				const double w = m_ui->edit_geom_w->text().toDouble();
				//material
				m_box->width(w);
				//edit
				m_ui->edit_geom_w->setText(QString::asprintf("%.2e", w));
				//draw
				m_ui->canvas->update();
			}
			void Box::slot_thick(void)
			{
				//data
				const double t = m_ui->edit_geom_t->text().toDouble();
				//material
				m_box->thickness(t);
				//edit
				m_ui->edit_geom_t->setText(QString::asprintf("%.2e", t));
				//draw
				m_ui->canvas->update();
			}
			void Box::slot_height(void)
			{
				//data
				const double h = m_ui->edit_geom_h->text().toDouble();
				//material
				m_box->height(h);
				//edit
				m_ui->edit_geom_h->setText(QString::asprintf("%.2e", h));
				//draw
				m_ui->canvas->update();
			}
			void Box::slot_mesh_w(void)
			{
				//data
				const int w = m_ui->edit_mesh_w->text().toInt();
				//material
				m_box->mesh_w(w);
				//edit
				m_ui->edit_mesh_w->setText(QString::asprintf("%02d", w));
				//draw
				m_ui->canvas->update();
			}
			void Box::slot_mesh_h(void)
			{
				//data
				const int h = m_ui->edit_mesh_h->text().toInt();
				//material
				m_box->mesh_h(h);
				//edit
				m_ui->edit_mesh_h->setText(QString::asprintf("%02d", h));
				//draw
				m_ui->canvas->update();
			}
			void Box::slot_mesh_t(void)
			{
				//data
				const int t = m_ui->edit_mesh_t->text().toInt();
				//material
				m_box->mesh_t(t);
				//edit
				m_ui->edit_mesh_t->setText(QString::asprintf("%02d", t));
				//draw
				m_ui->canvas->update();
			}
			void Box::slot_rebars(void)
			{
				//data
				Rebars(m_box, m_ui->canvas).exec();
				//draw
				m_ui->canvas->update();
			}
		}
	}
}
