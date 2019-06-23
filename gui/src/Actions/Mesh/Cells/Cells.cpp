//mat
#include "misc/util.h"

//fea
#include "Models/Model.h"

#include "Mesh/Mesh.h"
#include "Mesh/Cells/Cells.h"

//gui
#include "Canvas/ModelCanvas.h"
#include "Actions/Mesh/Cells/Cells.h"

//ui
#include "ui_Cells.h"

namespace gui
{
	namespace mesh
	{
		namespace cells
		{
			//constructors
			Cells::Cells(fea::mesh::Mesh* mesh, QWidget* parent) : QDialog(parent), m_ui(new Ui::Cells), m_mesh(mesh)
			{
				//set ui
				m_ui->setupUi(this);
				const unsigned nc = m_mesh->cells();
				//set combo type
				for(unsigned i = 1; i < unsigned(fea::mesh::cells::type::last); i <<= 1)
				{
					m_ui->combo_type->addItem(fea::mesh::cells::Cell::name(fea::mesh::cells::type(i)));
				}
				//set combo_index
				if(nc == 0)
				{
					m_ui->table->setEnabled(false);
					m_ui->edit_name->setEnabled(false);
					m_ui->push_remove->setEnabled(false);
					m_ui->combo_index->setEnabled(false);
					m_ui->slider_rule->setEnabled(false);
				}
				else
				{
					for (unsigned i = 0; i < nc; i++)
					{
						m_ui->combo_index->addItem(QString::asprintf("%02d", i + 1));
					}
				}
				//slider
				m_ui->slider_rule->setMinimum(1);
				m_ui->slider_rule->setMaximum(fea::mesh::cells::Cell::max_rule());
				//set table
				m_ui->table->setRowCount(nc);
				m_ui->table->setColumnCount(4);
				m_ui->table->setHorizontalHeaderItem(0, new QTableWidgetItem("Type"));
				m_ui->table->setHorizontalHeaderItem(1, new QTableWidgetItem("Name"));
				m_ui->table->setHorizontalHeaderItem(3, new QTableWidgetItem("Rule"));
				m_ui->table->setHorizontalHeaderItem(2, new QTableWidgetItem("Dimensions"));
				m_ui->table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
				for(unsigned i = 0; i < nc; i++)
				{
					add_cell(i);
				}
				//slots
				QObject::connect(m_ui->push_add, SIGNAL(clicked(bool)), SLOT(slot_add(void)));
				QObject::connect(m_ui->table, SIGNAL(cellClicked(int, int)), SLOT(slot_click(int)));
				QObject::connect(m_ui->push_remove, SIGNAL(clicked(bool)), SLOT(slot_remove(void)));
				QObject::connect(m_ui->edit_name, SIGNAL(editingFinished(void)), SLOT(slot_name(void)));
				QObject::connect(m_ui->slider_rule, SIGNAL(valueChanged(int)), SLOT(slot_slider(void)));
				QObject::connect(m_ui->combo_index, SIGNAL(currentIndexChanged(int)), SLOT(slot_index(int)));
			}

			//destructors
			Cells::~Cells(void)
			{
				delete m_ui;
			}

			//slots
			void Cells::slot_add(void)
			{
				//add cell
				const unsigned nc = m_mesh->cells() + 1;
				const unsigned st = m_ui->combo_type->currentIndex();
				fea::mesh::cells::Cell* cell = m_mesh->add_cell(fea::mesh::cells::type(1 << st));
				//set select
				m_ui->combo_index->setEnabled(true);
				m_ui->combo_index->addItem(QString::asprintf("%02d", nc));
				m_ui->combo_index->setCurrentIndex(nc - 1);
				//set edit
				m_ui->edit_name->setEnabled(true);
				m_ui->edit_name->setText(cell->label());
				//set push
				m_ui->push_remove->setEnabled(true);
				//set slider
				m_ui->slider_rule->setEnabled(true);
				//set table
				m_ui->table->setRowCount(nc);
				m_ui->table->setEnabled(true);
				//add cell
				add_cell(nc - 1);
			}
			void Cells::slot_name(void)
			{
				QString text = m_ui->edit_name->text();
				const unsigned i = m_ui->combo_index->currentIndex();
				m_mesh->cell(i)->label(text.remove(' ').toStdString().c_str());
				m_ui->table->item(i, 1)->setText(text);
				m_ui->edit_name->setText(text);
			}
			void Cells::slot_click(int i)
			{
				m_ui->combo_index->setCurrentIndex(i);
			}
			void Cells::slot_index(int i)
			{
				if(i != -1)
				{
					m_ui->edit_name->setText(m_mesh->cell(i)->label());
					m_ui->slider_rule->setValue(m_mesh->cell(i)->rule());
					m_ui->combo_type->setCurrentIndex(mat::log2(unsigned(m_mesh->cell(i)->type())));
				}
			}
			void Cells::slot_slider(void)
			{
				//index
				const unsigned i = (unsigned) m_ui->combo_index->currentIndex();
				//value
				m_mesh->cell(i)->rule(m_ui->slider_rule->value());
				//update
				update_cell(i);
			}
			void Cells::slot_remove(void)
			{
				//index
				const unsigned i = (unsigned) m_ui->combo_index->currentIndex();
				//remove section
				m_mesh->remove_cell(i);
				const unsigned nc = m_mesh->cells();
				//set combo
				m_ui->combo_index->removeItem(i);
				m_ui->combo_index->setEnabled(nc != 0);
				for(unsigned j = i; j < nc; j++)
				{
					m_ui->combo_index->setItemText(j, QString::asprintf("%02d", j + 1));
				}
				//set table
				m_ui->table->removeRow(i);
				m_ui->table->setEnabled(nc != 0);
				//set edit
				m_ui->edit_name->setEnabled(nc != 0);
				//set push
				m_ui->push_remove->setEnabled(nc != 0);
				//set slider
				m_ui->slider_rule->setEnabled(nc != 0);
			}
			
			//misc
			void Cells::add_cell(unsigned i) const
			{
				//section
				fea::mesh::cells::Cell* cell = m_mesh->cell(i);
				//type
				QTableWidgetItem* it = new QTableWidgetItem(cell->name());
				it->setFlags(it->flags() ^ Qt::ItemIsEditable);
				it->setTextAlignment(Qt::AlignCenter);
				m_ui->table->setItem(i, 0, it);
				//name
				QTableWidgetItem* in = new QTableWidgetItem(cell->label());
				in->setFlags(in->flags() ^ Qt::ItemIsEditable);
				in->setTextAlignment(Qt::AlignCenter);
				m_ui->table->setItem(i, 1, in);
				//rule
				QTableWidgetItem* ir = new QTableWidgetItem(QString::asprintf("%02d", cell->rule()));
				ir->setFlags(ir->flags() ^ Qt::ItemIsEditable);
				ir->setTextAlignment(Qt::AlignCenter);
				m_ui->table->setItem(i, 3, ir);
				//dimension
				QTableWidgetItem* id = new QTableWidgetItem(QString::asprintf("%dD", cell->dimension()));
				id->setFlags(ir->flags() ^ Qt::ItemIsEditable);
				id->setTextAlignment(Qt::AlignCenter);
				m_ui->table->setItem(i, 2, id);
			}
			void Cells::update_cell(unsigned i) const
			{
				//section
				fea::mesh::cells::Cell* cell = m_mesh->cell(i);
				//items
				if(i < m_ui->table->rowCount())
				{
					m_ui->table->item(i, 1)->setText(cell->label());
					m_ui->table->item(i, 3)->setText(QString::asprintf("%02d", cell->rule()));
				}
			}
		}
	}
}
