//fea
#include "Mesh/Mesh.h"
#include "Mesh/Nodes/Node.h"

#include "Model/Model.h"

//gui
#include "Canvas/Canvas.h"
#include "Actions/Mesh/Nodes/Nodes.h"

//ui
#include "ui_Nodes.h"

namespace gui
{
	namespace mesh
	{
		namespace nodes
		{
			//constructors
			Nodes::Nodes(fea::mesh::Mesh* mesh, gui::canvas::Canvas* canvas, QWidget* parent) : 
				QDialog(parent), m_ui(new Ui::Nodes), m_mesh(mesh), m_canvas(canvas)
			{
				//set ui
				m_ui->setupUi(this);
				const unsigned nn = m_mesh->nodes().size();
				//set combo
				if(nn == 0)
				{
					m_ui->table->setEnabled(false);
					m_ui->combo->setEnabled(false);
					m_ui->edit_x->setEnabled(false);
					m_ui->edit_y->setEnabled(false);
					m_ui->edit_z->setEnabled(false);
					m_ui->push_remove->setEnabled(false);
				}
				else
				{
					for (unsigned i = 0; i < nn; i++)
					{
						m_ui->combo->addItem(QString::asprintf("%05d", i + 1));
					}
				}
				//set edit
				if(nn != 0)
				{
					const double* p = m_mesh->node(0)->coordinates();
					m_ui->edit_x->setText(QString::asprintf("%+.2e", p[0]));
					m_ui->edit_y->setText(QString::asprintf("%+.2e", p[1]));
					m_ui->edit_z->setText(QString::asprintf("%+.2e", p[2]));
				}
				m_ui->edit_x->setValidator(new QDoubleValidator);
				m_ui->edit_y->setValidator(new QDoubleValidator);
				m_ui->edit_z->setValidator(new QDoubleValidator);
				//set table
				m_ui->table->setRowCount(nn);
				m_ui->table->setColumnCount(3);
				m_ui->table->setHorizontalHeaderItem(0, new QTableWidgetItem("X"));
				m_ui->table->setHorizontalHeaderItem(1, new QTableWidgetItem("Y"));
				m_ui->table->setHorizontalHeaderItem(2, new QTableWidgetItem("Z"));
				m_ui->table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
				for(unsigned i = 0; i < nn; i++)
				{
					//items
					const double* p = m_mesh->node(i)->coordinates();
					QTableWidgetItem* ix = new QTableWidgetItem(QString::asprintf("%+.2e", p[0]));
					QTableWidgetItem* iy = new QTableWidgetItem(QString::asprintf("%+.2e", p[1]));
					QTableWidgetItem* iz = new QTableWidgetItem(QString::asprintf("%+.2e", p[2]));
					//add
					m_ui->table->setItem(i, 0, ix);
					m_ui->table->setItem(i, 1, iy);
					m_ui->table->setItem(i, 2, iz);
					ix->setTextAlignment(Qt::AlignCenter);
					iy->setTextAlignment(Qt::AlignCenter);
					iz->setTextAlignment(Qt::AlignCenter);
				}
				//slots
				QObject::connect(m_ui->push_add, SIGNAL(clicked(bool)), SLOT(slot_add(void)));
				QObject::connect(m_ui->table, SIGNAL(cellClicked(int, int)), SLOT(slot_click(int)));
				QObject::connect(m_ui->edit_x, SIGNAL(editingFinished(void)), SLOT(slot_edit(void)));
				QObject::connect(m_ui->edit_y, SIGNAL(editingFinished(void)), SLOT(slot_edit(void)));
				QObject::connect(m_ui->edit_z, SIGNAL(editingFinished(void)), SLOT(slot_edit(void)));
				QObject::connect(m_ui->push_remove, SIGNAL(clicked(bool)), SLOT(slot_remove(void)));
				QObject::connect(m_ui->combo, SIGNAL(currentIndexChanged(int)), SLOT(slot_index(int)));
				QObject::connect(m_ui->table, SIGNAL(cellChanged(int, int)), SLOT(slot_table(int, int)));
			}

			//destructors
			Nodes::~Nodes(void)
			{
				delete m_ui;
			}

			//slots
			void Nodes::slot_add(void)
			{
				//add node
				m_mesh->model()->mark();
				m_mesh->add_node(0, 0, 0);
				const unsigned nn = m_mesh->nodes().size();
				//set select
				m_ui->combo->setEnabled(true);
				m_ui->combo->addItem(QString::asprintf("%04d", nn));
				m_ui->combo->setCurrentIndex(nn - 1);
				//set remove
				m_ui->push_remove->setEnabled(true);
				//set edits
				m_ui->edit_x->setEnabled(true);
				m_ui->edit_y->setEnabled(true);
				m_ui->edit_z->setEnabled(true);
				//set table
				m_ui->table->setRowCount(nn);
				m_ui->table->setEnabled(true);
				for(unsigned j = 0; j < 3; j++)
				{
					//create item
					QTableWidgetItem* it = new QTableWidgetItem(QString::asprintf("%+.2e", 0.0));
					//add item
					m_ui->table->setItem(nn - 1, j, it);
					it->setTextAlignment(Qt::AlignCenter);
				}
				//canvas
				m_canvas->bound();
				m_canvas->redraw();
			}
			void Nodes::slot_edit(void)
			{
				//data
				const unsigned i = (unsigned) m_ui->combo->currentIndex();
				QLineEdit* e[] = {m_ui->edit_x, m_ui->edit_y, m_ui->edit_z};
				//set ui
				m_mesh->model()->mark();
				for(unsigned j = 0; j < 3; j++)
				{
					if(e[j] == QObject::sender())
					{
						//set coordinates
						const double v = e[j]->text().toDouble();
						m_mesh->node(i)->coordinates(v, j);
						//set edit
						e[j]->setText(QString::asprintf("%+.2e", v));
						//set table
						m_ui->table->item(i, j)->setText(QString::asprintf("%+.2e", v));
					}
				}
				//canvas
				m_canvas->bound();
				m_canvas->redraw();
			}
			void Nodes::slot_click(int i)
			{
				m_ui->combo->setCurrentIndex(i);
			}
			void Nodes::slot_index(int i)
			{
				//check index
				if(i == -1)
				{
					return;
				}
				//data
				const double* p = m_mesh->node(i)->coordinates();
				//set edits
				m_ui->edit_x->setText(QString::asprintf("%+.2e", p[0]));
				m_ui->edit_y->setText(QString::asprintf("%+.2e", p[1]));
				m_ui->edit_z->setText(QString::asprintf("%+.2e", p[2]));
			}
			void Nodes::slot_remove(void)
			{
				//indes
				const unsigned i = (unsigned) m_ui->combo->currentIndex();
				//remove node
				m_mesh->remove_node(i);
				m_mesh->model()->mark();
				const unsigned n = m_mesh->nodes().size();
				//set combo
				m_ui->combo->removeItem(i);
				m_ui->combo->setEnabled(n != 0);
				for(unsigned j = i; j < n; j++)
				{
					m_ui->combo->setItemText(j, QString::asprintf("%04d", j + 1));
				}
				//set edits
				m_ui->edit_x->setEnabled(n != 0);
				m_ui->edit_y->setEnabled(n != 0);
				m_ui->edit_z->setEnabled(n != 0);
				//set table
				m_ui->table->removeRow(i);
				m_ui->table->setEnabled(n != 0);
				//canvas
				m_canvas->bound();
				m_canvas->redraw();
			}
			void Nodes::slot_table(int i, int j)
			{
				//data
				bool test;
				const double d = m_ui->table->item(i, j)->text().toDouble(&test);
				//node
				m_mesh->model()->mark();
				m_mesh->node(i)->coordinates(d, j);
				//edits
				QLineEdit* e[] = {m_ui->edit_x, m_ui->edit_y, m_ui->edit_z};
				e[j]->setText(QString::asprintf("%+.2e", d));
				//table
				m_ui->table->item(i, j)->setText(QString::asprintf("%+.2e", d));
				//canvas
				m_canvas->bound();
				m_canvas->redraw();
			}
		}
	}
}
