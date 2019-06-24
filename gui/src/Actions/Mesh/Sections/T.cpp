//std
#include <cmath>

//qt
#include <QDoubleValidator>

//fea
#include "Model/Model.h"

#include "Mesh/Mesh.h"
#include "Mesh/Sections/T.h"

//gui
#include "Actions/Mesh/Sections/T.h"
#include "Actions/Mesh/Sections/Rebars.h"

//ui
#include "ui_T.h"

namespace gui
{
	namespace mesh
	{
		namespace sections
		{
			//constructors
			T::T(fea::mesh::sections::T* t, QWidget* parent) : QDialog(parent), m_ui(new Ui::T), m_t(t)
			{
				//set ui
				m_ui->setupUi(this);
				m_ui->canvas->section(m_t);
				setWindowTitle(QString::asprintf("T - Index: %02d - Name: %s", m_t->index() + 1, m_t->label()));
				//set edit
				m_ui->edit_name->setText(m_t->label());
				m_ui->edit_mesh_wh->setText(QString::asprintf("%02d", m_t->mesh_wh()));
				m_ui->edit_mesh_wt->setText(QString::asprintf("%02d", m_t->mesh_wt()));
				m_ui->edit_mesh_fw->setText(QString::asprintf("%02d", m_t->mesh_fw()));
				m_ui->edit_mesh_ft->setText(QString::asprintf("%02d", m_t->mesh_ft()));
				m_ui->edit_geom_wh->setText(QString::asprintf("%.2e", m_t->web_height()));
				m_ui->edit_geom_fw->setText(QString::asprintf("%.2e", m_t->flange_width()));
				m_ui->edit_geom_wt->setText(QString::asprintf("%.2e", m_t->web_thickness()));
				m_ui->edit_geom_ft->setText(QString::asprintf("%.2e", m_t->flange_thickness()));
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
			T::~T(void)
			{
				delete m_ui;
			}

			//slots
			void T::slot_name(void)
			{
				m_t->mesh()->model()->mark();
				QString text = m_ui->edit_name->text();
				m_t->label(text.remove(' ').toStdString().c_str());
				m_ui->edit_name->setText(text);
			}
			void T::slot_rebars(void)
			{
				//data
				Rebars(m_t, m_ui->canvas).exec();
				//draw
				m_ui->canvas->update();
			}
			void T::slot_mesh_wh(void)
			{
				//data
				const int v = m_ui->edit_mesh_wh->text().toInt();
				//material
				m_t->mesh_wh(v);
				m_t->mesh()->model()->mark();
				//edit
				m_ui->edit_mesh_wh->setText(QString::asprintf("%02d", v));
				//draw
				m_ui->canvas->update();
			}
			void T::slot_mesh_wt(void)
			{
				//data
				const int v = m_ui->edit_mesh_wt->text().toInt();
				//material
				m_t->mesh_wt(v);
				m_t->mesh()->model()->mark();
				//edit
				m_ui->edit_mesh_wt->setText(QString::asprintf("%02d", v));
				//draw
				m_ui->canvas->update();
			}
			void T::slot_mesh_fw(void)
			{
				//data
				const int v = m_ui->edit_mesh_fw->text().toInt();
				//material
				m_t->mesh_fw(v);
				m_t->mesh()->model()->mark();
				//edit
				m_ui->edit_mesh_fw->setText(QString::asprintf("%02d", v));
				//draw
				m_ui->canvas->update();
			}
			void T::slot_mesh_ft(void)
			{
				//data
				const int v = m_ui->edit_mesh_ft->text().toInt();
				//material
				m_t->mesh_ft(v);
				m_t->mesh()->model()->mark();
				//edit
				m_ui->edit_mesh_ft->setText(QString::asprintf("%02d", v));
				//draw
				m_ui->canvas->update();
			}
			void T::slot_geom_wh(void)
			{
				//data
				const double v = m_ui->edit_geom_wh->text().toDouble();
				//material
				m_t->web_height(v);
				m_t->mesh()->model()->mark();
				//edit
				m_ui->edit_geom_wh->setText(QString::asprintf("%.2e", v));
				//draw
				m_ui->canvas->update();
			}
			void T::slot_geom_wt(void)
			{
				//data
				const double v = m_ui->edit_geom_wt->text().toDouble();
				//material
				m_t->web_thickness(v);
				m_t->mesh()->model()->mark();
				//edit
				m_ui->edit_geom_wt->setText(QString::asprintf("%.2e", v));
				//draw
				m_ui->canvas->update();
			}
			void T::slot_geom_fw(void)
			{
				//data
				const double v = m_ui->edit_geom_fw->text().toDouble();
				//material
				m_t->flange_width(v);
				m_t->mesh()->model()->mark();
				//edit
				m_ui->edit_geom_fw->setText(QString::asprintf("%.2e", v));
				//draw
				m_ui->canvas->update();
			}
			void T::slot_geom_ft(void)
			{
				//data
				const double v = m_ui->edit_geom_ft->text().toDouble();
				//material
				m_t->flange_thickness(v);
				m_t->mesh()->model()->mark();
				//edit
				m_ui->edit_geom_ft->setText(QString::asprintf("%.2e", v));
				//draw
				m_ui->canvas->update();
			}
		}
	}
}
