//std
#include <cmath>

//qt
#include <QMessageBox>
#include <QDoubleValidator>

//mat
#include "misc/util.h"

//fea
#include "Mesh/Mesh.h"
#include "Mesh/Elements/Elements.h"

//gui
#include "Actions/Mesh/Elements/ElementsMesh.h"

//ui
#include "ui_ElementsMesh.h"

namespace gui
{
	namespace mesh
	{
		namespace elements
		{
			//constructors
			Elements::Elements(fea::mesh::Mesh* mesh, gui::canvas::Model* canvas, QWidget* parent) : 
				QDialog(parent), m_ui(new Ui::Elements), m_mesh(mesh), m_canvas(canvas)
			{
				//set ui
				m_ui->setupUi(this);
				const unsigned ne = m_mesh->elements();
				//set combo type
				for(unsigned i = 1; i < unsigned(fea::mesh::elements::type::last); i <<= 1)
				{
					m_ui->combo_type->addItem(fea::mesh::elements::Element::name(fea::mesh::elements::type(i)));
				}
				//set combo index
				if(ne == 0)
				{
					m_ui->table->setEnabled(false);
					m_ui->combo_index->setEnabled(false);
					m_ui->button_remove->setEnabled(false);
				}
				else
				{
					for (unsigned i = 0; i < ne; i++)
					{
						m_ui->combo_index->addItem(QString::asprintf("%02d", i + 1));
					}
				}
				//set table
				m_ui->table->setRowCount(ne);
				m_ui->table->setColumnCount(4);
				m_ui->table->setHorizontalHeaderItem(0, new QTableWidgetItem("Type"));
				m_ui->table->setHorizontalHeaderItem(1, new QTableWidgetItem("Cell"));
				m_ui->table->setHorizontalHeaderItem(3, new QTableWidgetItem("Nodes"));
				m_ui->table->setHorizontalHeaderItem(2, new QTableWidgetItem("Material"));
				m_ui->table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
				for(unsigned i = 0; i < ne; i++)
				{
					table_add(i);
				}
				//slots
				QObject::connect(m_ui->button_add, SIGNAL(clicked(bool)), SLOT(slot_add(void)));
				QObject::connect(m_ui->table, SIGNAL(cellClicked(int, int)), SLOT(slot_click(int)));
				QObject::connect(m_ui->button_remove, SIGNAL(clicked(bool)), SLOT(slot_remove(void)));
				QObject::connect(m_ui->combo_index, SIGNAL(currentIndexChanged(int)), SLOT(slot_index(int)));
			}

			//destructors
			Elements::~Elements(void)
			{
				delete m_ui;
			}

			//slots
			void Elements::slot_add(void)
			{
				//check nodes
				if(m_mesh->nodes() == 0)
				{
					QMessageBox::warning(nullptr, "Warning", "The current mesh has an empty list of nodes!", QMessageBox::Ok);
					return;
				}
				//check cells
				if(m_mesh->cells() == 0)
				{
					QMessageBox::warning(nullptr, "Warning", "The current mesh has an empty list of cells!", QMessageBox::Ok);
					return;
				}
				//check materials
				if(m_mesh->materials() == 0)
				{
					QMessageBox::warning(nullptr, "Warning", "The current mesh has an empty list of materials!", QMessageBox::Ok);
					return;
				}
				//add element
				const unsigned ne = m_mesh->elements() + 1;
				const unsigned st = m_ui->combo_type->currentIndex();
				m_mesh->add_element(fea::mesh::elements::type(1 << st), {}, 0, 0);
				//set select
				m_ui->combo_index->setEnabled(true);
				m_ui->combo_index->addItem(QString::asprintf("%02d", ne));
				m_ui->combo_index->setCurrentIndex(ne - 1);
				//set remove
				m_ui->button_remove->setEnabled(true);
				//set table
				m_ui->table->setRowCount(ne);
				m_ui->table->setEnabled(true);
				table_add(ne - 1);
			}
			void Elements::slot_click(int i)
			{
				switch(m_mesh->element(i)->type())
				{
					default:
						return;
				}
				table_update(i);
			}
			void Elements::slot_index(int i)
			{
				if(i != -1)
				{
					m_ui->table->selectRow(i);
					m_ui->combo_type->setCurrentIndex(mat::log2(unsigned(m_mesh->element(i)->type())));
				}
			}
			void Elements::slot_remove(void)
			{
				//index
				const unsigned i = (unsigned) m_ui->combo_index->currentIndex();
				//remove element
				m_mesh->remove_element(i);
				const unsigned ne = m_mesh->elements();
				//set combo
				m_ui->combo_index->removeItem(i);
				m_ui->combo_index->setEnabled(ne != 0);
				for(unsigned j = i; j < ne; j++)
				{
					m_ui->combo_index->setItemText(j, QString::asprintf("%02d", j + 1));
				}
				//set table
				m_ui->table->removeRow(i);
				m_ui->table->setEnabled(ne != 0);
				//set buttons
				m_ui->button_remove->setEnabled(ne != 0);
			}
			
			//add
			void Elements::table_add(unsigned i) const
			{
				
			}
			void Elements::table_update(unsigned i) const
			{
				
			}
		}
	}
}
