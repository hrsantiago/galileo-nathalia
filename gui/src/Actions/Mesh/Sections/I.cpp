//std
#include <cmath>
#include <QIntValidator>
#include <QDoubleValidator>

//fea
#include "Mesh/Mesh.h"
#include "Mesh/Sections/I.h"

//gui
#include "Actions/Mesh/Sections/I.h"
#include "Actions/Mesh/Sections/Rebars.h"

//ui
#include "ui_I.h"

namespace gui
{
	namespace mesh
	{
		namespace sections
		{
			//constructors
			I::I(fea::mesh::sections::I* i, QWidget* parent) : QDialog(parent), m_ui(new Ui::I), m_i(i)
			{
				//set ui
				m_ui->setupUi(this);
				m_ui->canvas->section(m_i);
				setWindowTitle(QString::asprintf("I - Index: %02d - Name: %s", m_i->index(), m_i->label()));
				//set edit
				m_ui->edit_name->setText(m_i->label());
				m_ui->edit_mesh_wh->setText(QString::asprintf("%02d", m_i->mesh_wh()));
				m_ui->edit_mesh_wt->setText(QString::asprintf("%02d", m_i->mesh_wt()));
				m_ui->edit_mesh_fw->setText(QString::asprintf("%02d", m_i->mesh_fw()));
				m_ui->edit_mesh_ft->setText(QString::asprintf("%02d", m_i->mesh_ft()));
				m_ui->edit_geom_wh->setText(QString::asprintf("%.2e", m_i->web_height()));
				m_ui->edit_geom_fw->setText(QString::asprintf("%.2e", m_i->flange_width()));
				m_ui->edit_geom_wt->setText(QString::asprintf("%.2e", m_i->web_thickness()));
				m_ui->edit_geom_ft->setText(QString::asprintf("%.2e", m_i->flange_thickness()));
				//validators
				m_ui->edit_mesh_wh->setValidator(new QIntValidator(0, std::numeric_limits<int>::max()));
				m_ui->edit_mesh_wt->setValidator(new QIntValidator(0, std::numeric_limits<int>::max()));
				m_ui->edit_mesh_fw->setValidator(new QIntValidator(0, std::numeric_limits<int>::max()));
				m_ui->edit_mesh_ft->setValidator(new QIntValidator(0, std::numeric_limits<int>::max()));
				m_ui->edit_geom_wh->setValidator(new QDoubleValidator(0, std::numeric_limits<double>::max(), 3));
				m_ui->edit_geom_fw->setValidator(new QDoubleValidator(0, std::numeric_limits<double>::max(), 3));
				m_ui->edit_geom_wt->setValidator(new QDoubleValidator(0, std::numeric_limits<double>::max(), 3));
				m_ui->edit_geom_ft->setValidator(new QDoubleValidator(0, std::numeric_limits<double>::max(), 3));
				//slots
				QObject::connect(m_ui->push_rebars, SIGNAL(clicked(void)), SLOT(slot_rebars(void)));
				QObject::connect(m_ui->edit_name, SIGNAL(editingFinished(void)), SLOT(slot_name(void)));
				QObject::connect(m_ui->edit_mesh_wh, SIGNAL(editingFinished(void)), SLOT(slot_mesh_wh(void)));
				QObject::connect(m_ui->edit_mesh_wt, SIGNAL(editingFinished(void)), SLOT(slot_mesh_wt(void)));
				QObject::connect(m_ui->edit_mesh_fw, SIGNAL(editingFinished(void)), SLOT(slot_mesh_fw(void)));
				QObject::connect(m_ui->edit_mesh_ft, SIGNAL(editingFinished(void)), SLOT(slot_mesh_ft(void)));
				QObject::connect(m_ui->edit_geom_wh, SIGNAL(editingFinished(void)), SLOT(slot_geom_wh(void)));
				QObject::connect(m_ui->edit_geom_wt, SIGNAL(editingFinished(void)), SLOT(slot_geom_wt(void)));
				QObject::connect(m_ui->edit_geom_fw, SIGNAL(editingFinished(void)), SLOT(slot_geom_fw(void)));
				QObject::connect(m_ui->edit_geom_ft, SIGNAL(editingFinished(void)), SLOT(slot_geom_ft(void)));
			}

			//destructors
			I::~I(void)
			{
				delete m_ui;
			}

			//slots
			void I::slot_name(void)
			{
				QString text = m_ui->edit_name->text();
				m_i->label(text.remove(' ').toStdString().c_str());
				m_ui->edit_name->setText(text);
			}
			void I::slot_rebars(void)
			{
				//data
				Rebars(m_i, m_ui->canvas).exec();
				//draw
				m_ui->canvas->update();
			}
			void I::slot_mesh_wh(void)
			{
				//data
				const int v = m_ui->edit_mesh_wh->text().toInt();
				//material
				m_i->mesh_wh(v);
				//edit
				m_ui->edit_mesh_wh->setText(QString::asprintf("%02d", v));
				//draw
				m_ui->canvas->update();
			}
			void I::slot_mesh_wt(void)
			{
				//data
				const int v = m_ui->edit_mesh_wt->text().toInt();
				//material
				m_i->mesh_wt(v);
				//edit
				m_ui->edit_mesh_wt->setText(QString::asprintf("%02d", v));
				//draw
				m_ui->canvas->update();
			}
			void I::slot_mesh_fw(void)
			{
				//data
				const int v = m_ui->edit_mesh_fw->text().toInt();
				//material
				m_i->mesh_fw(v);
				//edit
				m_ui->edit_mesh_fw->setText(QString::asprintf("%02d", v));
				//draw
				m_ui->canvas->update();
			}
			void I::slot_mesh_ft(void)
			{
				//data
				const int v = m_ui->edit_mesh_ft->text().toInt();
				//material
				m_i->mesh_ft(v);
				//edit
				m_ui->edit_mesh_ft->setText(QString::asprintf("%02d", v));
				//draw
				m_ui->canvas->update();
			}
			void I::slot_geom_wh(void)
			{
				//data
				const double v = m_ui->edit_geom_wh->text().toDouble();
				//material
				m_i->web_height(v);
				//edit
				m_ui->edit_geom_wh->setText(QString::asprintf("%.2e", v));
				//draw
				m_ui->canvas->update();
			}
			void I::slot_geom_wt(void)
			{
				//data
				const double v = m_ui->edit_geom_wt->text().toDouble();
				//material
				m_i->web_thickness(v);
				//edit
				m_ui->edit_geom_wt->setText(QString::asprintf("%.2e", v));
				//draw
				m_ui->canvas->update();
			}
			void I::slot_geom_fw(void)
			{
				//data
				const double v = m_ui->edit_geom_fw->text().toDouble();
				//material
				m_i->flange_width(v);
				//edit
				m_ui->edit_geom_fw->setText(QString::asprintf("%.2e", v));
				//draw
				m_ui->canvas->update();
			}
			void I::slot_geom_ft(void)
			{
				//data
				const double v = m_ui->edit_geom_ft->text().toDouble();
				//material
				m_i->flange_thickness(v);
				//edit
				m_ui->edit_geom_ft->setText(QString::asprintf("%.2e", v));
				//draw
				m_ui->canvas->update();
			}
		}
	}
}
