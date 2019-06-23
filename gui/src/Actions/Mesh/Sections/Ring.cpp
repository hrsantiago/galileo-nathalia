//std
#include <cmath>
#include <QIntValidator>
#include <QDoubleValidator>

//fea
#include "Mesh/Mesh.h"
#include "Mesh/Sections/Ring.h"

//gui
#include "Actions/Mesh/Sections/Ring.h"
#include "Actions/Mesh/Sections/Rebars.h"

//ui
#include "ui_Ring.h"

namespace gui
{
	namespace mesh
	{
		namespace sections
		{
			//constructors
			Ring::Ring(fea::mesh::sections::Ring* ring, QWidget* parent) : QDialog(parent), m_ui(new Ui::Ring), m_ring(ring)
			{
				//set ui
				m_ui->setupUi(this);
				m_ui->canvas->section(m_ring);
				setWindowTitle(QString::asprintf("Ring - Index: %02d - Name: %s", m_ring->index(), m_ring->label()));
				//set edit
				m_ui->edit_name->setText(m_ring->label());
				m_ui->edit_mesh_r->setText(QString::asprintf("%02d", m_ring->mesh_r()));
				m_ui->edit_mesh_t->setText(QString::asprintf("%02d", m_ring->mesh_t()));
				m_ui->edit_geom_d->setText(QString::asprintf("%.2e", m_ring->diameter()));
				m_ui->edit_geom_t->setText(QString::asprintf("%.2e", m_ring->thickness()));
				//validators
				m_ui->edit_mesh_r->setValidator(new QIntValidator(0, std::numeric_limits<int>::max()));
				m_ui->edit_mesh_t->setValidator(new QIntValidator(0, std::numeric_limits<int>::max()));
				m_ui->edit_geom_d->setValidator(new QDoubleValidator(0, std::numeric_limits<double>::max(), 3));
				//slots
				QObject::connect(m_ui->push_rebars, SIGNAL(clicked(void)), SLOT(slot_rebars(void)));
				QObject::connect(m_ui->edit_name, SIGNAL(editingFinished(void)), SLOT(slot_name(void)));
				QObject::connect(m_ui->edit_geom_t, SIGNAL(editingFinished(void)), SLOT(slot_thick(void)));
				QObject::connect(m_ui->edit_mesh_r, SIGNAL(editingFinished(void)), SLOT(slot_mesh_r(void)));
				QObject::connect(m_ui->edit_mesh_t, SIGNAL(editingFinished(void)), SLOT(slot_mesh_t(void)));
				QObject::connect(m_ui->edit_geom_d, SIGNAL(editingFinished(void)), SLOT(slot_diameter(void)));
			}

			//destructors
			Ring::~Ring(void)
			{
				delete m_ui;
			}

			//slots
			void Ring::slot_name(void)
			{
				QString text = m_ui->edit_name->text();
				m_ring->label(text.remove(' ').toStdString().c_str());
				m_ui->edit_name->setText(text);
			}
			void Ring::slot_thick(void)
			{
				//data
				const double t = m_ui->edit_geom_t->text().toDouble();
				//material
				m_ring->thickness(t);
				//edit
				m_ui->edit_geom_t->setText(QString::asprintf("%.2e", t));
				//draw
				m_ui->canvas->update();
			}
			void Ring::slot_mesh_r(void)
			{
				//data
				const int r = m_ui->edit_mesh_r->text().toInt();
				//material
				m_ring->mesh_r(r);
				//edit
				m_ui->edit_mesh_r->setText(QString::asprintf("%02d", r));
				//draw
				m_ui->canvas->update();
			}
			void Ring::slot_mesh_t(void)
			{
				//data
				const int t = m_ui->edit_mesh_t->text().toInt();
				//material
				m_ring->mesh_t(t);
				//edit
				m_ui->edit_mesh_t->setText(QString::asprintf("%02d", t));
				//draw
				m_ui->canvas->update();
			}
			void Ring::slot_rebars(void)
			{
				//data
				Rebars(m_ring, m_ui->canvas).exec();
				//draw
				m_ui->canvas->update();
			}
			void Ring::slot_diameter(void)
			{
				//data
				const double d = m_ui->edit_geom_d->text().toDouble();
				//material
				m_ring->diameter(d);
				//edit
				m_ui->edit_geom_d->setText(QString::asprintf("%.2e", d));
				//draw
				m_ui->canvas->update();
			}
		}
	}
}
