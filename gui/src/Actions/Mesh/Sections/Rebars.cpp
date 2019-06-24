//std
#include <cmath>

//fea
#include "Model/Model.h"

#include "Mesh/Mesh.h"
#include "Mesh/Sections/Rebar.h"
#include "Mesh/Sections/Sections.h"

//gui
#include "Canvas/Section.h"
#include "Actions/Mesh/Sections/Rebars.h"

//ui
#include "ui_Rebars.h"

namespace gui
{
	namespace mesh
	{
		namespace sections
		{
			//constructors
			Rebars::Rebars(fea::mesh::sections::Section* section, gui::canvas::Section* canvas, QWidget* parent) : QDialog(parent), 
			m_ui(new Ui::Rebars), m_section(section), m_canvas(canvas)
			{
				//set ui
				m_ui->setupUi(this);
				const unsigned nr = m_section->rebars();
				setWindowTitle(QString("Section Rebars - Index: %1 - Name: %2").arg(m_section->index() + 1, 2, 10, QChar('0')).arg(m_section->label()));
				//set widgets
				m_ui->table->setEnabled(nr != 0);
				m_ui->push_remove->setEnabled(nr != 0);
				//set table
				m_ui->table->setRowCount(nr);
				m_ui->table->setColumnCount(3);
				m_ui->table->setHorizontalHeaderItem(0, new QTableWidgetItem("d"));
				m_ui->table->setHorizontalHeaderItem(1, new QTableWidgetItem("z"));
				m_ui->table->setHorizontalHeaderItem(2, new QTableWidgetItem("y"));
				m_ui->table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
				for(unsigned i = 0; i < nr; i++)
				{
					table_add(i);
				}
				//slots
				QObject::connect(m_ui->push_add, SIGNAL(clicked(bool)), SLOT(slot_add(void)));
				QObject::connect(m_ui->push_remove, SIGNAL(clicked(bool)), SLOT(slot_remove(void)));
				QObject::connect(m_ui->table, SIGNAL(cellChanged(int, int)), SLOT(slot_table(int, int)));
			}

			//destructors
			Rebars::~Rebars(void)
			{
				delete m_ui;
			}

			//slots
			void Rebars::slot_add(void)
			{
				//add node
				m_section->add_rebar(0, 0, 0);
				m_section->mesh()->model()->mark();
				const unsigned nr = m_section->rebars();
				//set remove
				m_ui->push_remove->setEnabled(true);
				//set table
				m_ui->table->setRowCount(nr);
				m_ui->table->setEnabled(true);
				m_ui->table->selectRow(nr - 1);
				table_add(nr - 1);
				//draw
				if(m_canvas)
				{
					m_canvas->update();
				}
			}
			void Rebars::slot_remove(void)
			{
				//index
				m_section->mesh()->model()->mark();
				const int i = (unsigned) m_ui->table->currentRow();
				if(i != -1)
				{
					//remove rebar
					m_section->remove_rebar(i);
					const unsigned nr = m_section->rebars();
					//set pushs
					m_ui->push_remove->setEnabled(nr != 0);
					//set table
					m_ui->table->removeRow(i);
					m_ui->table->selectRow(nr - 1);
					m_ui->table->setEnabled(nr != 0);
				}
				//draw
				if(m_canvas)
				{
					m_canvas->update();
				}
			}
			void Rebars::slot_table(int i, int j)
			{
				//data
				bool test;
				const double d = m_ui->table->item(i, j)->text().toDouble(&test);
				const double v = test ? j == 0 ? fabs(d) : d : 0;
				//rebar
				switch(j)
				{
					case 0:
						m_section->rebar(i).diameter(v);
						break;
					case 1:
						m_section->rebar(i).position_z(v);
						break;
					case 2:
						m_section->rebar(i).position_y(v);
						break;
				}
				//table
				m_section->mesh()->model()->mark();
				m_ui->table->item(i, j)->setText(QString::asprintf("%+.2e", v));
				//draw
				if(m_canvas)
				{
					m_canvas->update();
				}
			}
			
			//table
			void Rebars::table_add(unsigned i) const
			{
				//rebar
				const fea::mesh::sections::Rebar& rebar = m_section->rebar(i);
				//items
				QTableWidgetItem* id = new QTableWidgetItem(QString("%1").arg(rebar.diameter(), 0, 'e', 2));
				QTableWidgetItem* iz = new QTableWidgetItem(QString("%1").arg(rebar.position_z(), 0, 'e', 2));
				QTableWidgetItem* iy = new QTableWidgetItem(QString("%1").arg(rebar.position_y(), 0, 'e', 2));
				//alignment
				id->setTextAlignment(Qt::AlignCenter);
				iz->setTextAlignment(Qt::AlignCenter);
				iy->setTextAlignment(Qt::AlignCenter);
				//table
				m_ui->table->setItem(i, 0, id);
				m_ui->table->setItem(i, 1, iz);
				m_ui->table->setItem(i, 2, iy);
			}
		}
	}
}
