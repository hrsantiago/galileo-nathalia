//std
#include <cmath>

//qt
#include <QDoubleValidator>

//fea
#include "Model/Model.h"

#include "Mesh/Mesh.h"
#include "Mesh/Materials/Heat/Heat.h"

//gui
#include "Actions/Mesh/Materials/Heat/Heat.h"

//ui
#include "ui_Heat.h"

namespace gui
{
	namespace mesh
	{
		namespace materials
		{
			//constructors
			Heat::Heat(fea::mesh::materials::Heat* heat, QWidget* parent) : QDialog(parent), m_ui(new Ui::Heat), m_heat(heat)
			{
				//set ui
				m_ui->setupUi(this);
				setWindowTitle(QString::asprintf("Heat - Index: %02d - Name: %s", m_heat->index() + 1, m_heat->label()));
				//set edits
				m_ui->edit_name->setText(m_heat->label());
				m_ui->edit_capa->setText(QString::asprintf("%.2e", m_heat->capacity()));
				m_ui->edit_cond->setText(QString::asprintf("%.2e", m_heat->condutivity()));
				m_ui->edit_mass->setText(QString::asprintf("%.2e", m_heat->specific_mass()));
				m_ui->edit_mass->setValidator(new QDoubleValidator(0, std::numeric_limits<double>::max(), 3));
				m_ui->edit_capa->setValidator(new QDoubleValidator(0, std::numeric_limits<double>::max(), 3));
				m_ui->edit_cond->setValidator(new QDoubleValidator(0, std::numeric_limits<double>::max(), 3));
				//slots
				QObject::connect(m_ui->edit_name, SIGNAL(editingFinished(void)), SLOT(slot_name(void)));
				QObject::connect(m_ui->edit_mass, SIGNAL(editingFinished(void)), SLOT(slot_mass(void)));
				QObject::connect(m_ui->edit_capa, SIGNAL(editingFinished(void)), SLOT(slot_capacity(void)));
				QObject::connect(m_ui->edit_cond, SIGNAL(editingFinished(void)), SLOT(slot_condutivity(void)));
			}

			//destructors
			Heat::~Heat(void)
			{
				delete m_ui;
			}

			//slots
			void Heat::slot_name(void)
			{
				m_heat->mesh()->model()->mark();
				QString text = m_ui->edit_name->text();
				m_heat->label(text.remove(" ").toStdString().c_str());
				m_ui->edit_name->setText(text);
			}
			void Heat::slot_mass(void)
			{
				//data
				const double m = m_ui->edit_mass->text().toDouble();
				//material
				m_heat->specific_mass(m);
				m_heat->mesh()->model()->mark();
				//edit
				m_ui->edit_mass->setText(QString::asprintf("%.2e", m));
			}
			void Heat::slot_capacity(void)
			{
				//data
				const double c = m_ui->edit_capa->text().toDouble();
				//material
				m_heat->capacity(c);
				m_heat->mesh()->model()->mark();
				//edit
				m_ui->edit_capa->setText(QString::asprintf("%.2e", c));
			}
			void Heat::slot_condutivity(void)
			{
				//data
				const double c = m_ui->edit_cond->text().toDouble();
				//material
				m_heat->condutivity(c);
				m_heat->mesh()->model()->mark();
				//edit
				m_ui->edit_cond->setText(QString::asprintf("%.2e", c));
			}
		}
	}
}
