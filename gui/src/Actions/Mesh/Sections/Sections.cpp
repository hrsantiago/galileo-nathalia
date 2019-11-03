//mat
#include "misc/util.h"

//fea
#include "Model/Model.h"

#include "Mesh/Mesh.h"
#include "Mesh/Sections/Types.h"
#include "Mesh/Sections/Section.h"

//gui
#include "Actions/Mesh/Sections/T.h"
#include "Actions/Mesh/Sections/I.h"
#include "Actions/Mesh/Sections/Box.h"
#include "Actions/Mesh/Sections/Ring.h"
#include "Actions/Mesh/Sections/Circle.h"
#include "Actions/Mesh/Sections/Rebars.h"
#include "Actions/Mesh/Sections/Sections.h"
#include "Actions/Mesh/Sections/Rectangle.h"

//ui
#include "ui_Sections.h"

namespace gui
{
	namespace mesh
	{
		namespace sections
		{
			//constructors
			Sections::Sections(fea::mesh::Mesh* mesh, QWidget* parent) : QDialog(parent), m_ui(new Ui::Sections), m_mesh(mesh)
			{
				//set ui
				m_ui->setupUi(this);
				const unsigned ns = m_mesh->sections().size();
				//set combo type
				for(unsigned i = 1; i < unsigned(fea::mesh::sections::type::last); i <<= 1)
				{
					m_ui->combo_type->addItem(fea::mesh::sections::Section::name(fea::mesh::sections::type(i)));
				}
				//set combo index
				if(ns == 0)
				{
					m_ui->table->setEnabled(false);
					m_ui->edit_name->setEnabled(false);
					m_ui->combo_index->setEnabled(false);
					m_ui->push_rebars->setEnabled(false);
					m_ui->push_remove->setEnabled(false);
				}
				else
				{
					for (unsigned i = 0; i < ns; i++)
					{
						m_ui->combo_index->addItem(QString::asprintf("%02d", i + 1));
					}
				}
				//set edit
				if(ns != 0)
				{
					m_ui->edit_name->setText(m_mesh->section(0)->label());
				}
				//set table
				m_ui->table->setRowCount(ns);
				m_ui->table->setColumnCount(13);
				m_ui->table->setHorizontalHeaderItem( 0, new QTableWidgetItem("Type"));
				m_ui->table->setHorizontalHeaderItem( 1, new QTableWidgetItem("Name"));
				m_ui->table->setHorizontalHeaderItem( 2, new QTableWidgetItem("Rebars"));
				m_ui->table->setHorizontalHeaderItem( 3, new QTableWidgetItem("A"));
				m_ui->table->setHorizontalHeaderItem( 4, new QTableWidgetItem("Iz"));
				m_ui->table->setHorizontalHeaderItem( 5, new QTableWidgetItem("Iy"));
				m_ui->table->setHorizontalHeaderItem( 6, new QTableWidgetItem("Ix"));
				m_ui->table->setHorizontalHeaderItem( 7, new QTableWidgetItem("Sz"));
				m_ui->table->setHorizontalHeaderItem( 8, new QTableWidgetItem("Sy"));
				m_ui->table->setHorizontalHeaderItem( 9, new QTableWidgetItem("Wz"));
				m_ui->table->setHorizontalHeaderItem(10, new QTableWidgetItem("Wy"));
				m_ui->table->setHorizontalHeaderItem(11, new QTableWidgetItem("rz"));
				m_ui->table->setHorizontalHeaderItem(12, new QTableWidgetItem("ry"));
				m_ui->table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
				for(unsigned i = 0; i < ns; i++)
				{
					table_add(i);
				}
				//slots
				QObject::connect(m_ui->push_add, SIGNAL(clicked(bool)), SLOT(slot_add(void)));
				QObject::connect(m_ui->table, SIGNAL(cellClicked(int, int)), SLOT(slot_click(int)));
				QObject::connect(m_ui->push_remove, SIGNAL(clicked(bool)), SLOT(slot_remove(void)));
				QObject::connect(m_ui->push_rebars, SIGNAL(clicked(bool)), SLOT(slot_rebars(void)));
				QObject::connect(m_ui->edit_name, SIGNAL(editingFinished(void)), SLOT(slot_name(void)));
				QObject::connect(m_ui->combo_index, SIGNAL(currentIndexChanged(int)), SLOT(slot_index(int)));
			}

			//destructors
			Sections::~Sections(void)
			{
				delete m_ui;
			}

			//slots
			void Sections::slot_add(void)
			{
				//add section
				m_mesh->model()->mark();
				const unsigned ns = m_mesh->sections().size() + 1;
				const unsigned st = m_ui->combo_type->currentIndex();
				fea::mesh::sections::Section* section = m_mesh->add_section(fea::mesh::sections::type(1 << st));
				//set select
				m_ui->combo_index->setEnabled(true);
				m_ui->combo_index->addItem(QString::asprintf("%02d", ns));
				m_ui->combo_index->setCurrentIndex(ns - 1);
				//set edit
				m_ui->edit_name->setEnabled(true);
				m_ui->edit_name->setText(section->label());
				//set pushs
				m_ui->push_rebars->setEnabled(true);
				m_ui->push_remove->setEnabled(true);
				//set table
				m_ui->table->setRowCount(ns);
				m_ui->table->setEnabled(true);
				table_add(ns - 1);
			}
			void Sections::slot_name(void)
			{
				m_mesh->model()->mark();
				QString text = m_ui->edit_name->text();
				const unsigned i = m_ui->combo_index->currentIndex();
				m_mesh->section(i)->label(text.remove(' ').toStdString().c_str());
				m_ui->table->item(i, 1)->setText(text);
				m_ui->edit_name->setText(text);
			}
			void Sections::slot_click(int i)
			{
				switch(m_mesh->section(i)->type())
				{
					case fea::mesh::sections::type::I:
						I((fea::mesh::sections::I*) m_mesh->section(i)).exec();
						break;
					case fea::mesh::sections::type::T:
						T((fea::mesh::sections::T*) m_mesh->section(i)).exec();
						break;
					case fea::mesh::sections::type::box:
						Box((fea::mesh::sections::Box*) m_mesh->section(i)).exec();
						break;
					case fea::mesh::sections::type::ring:
						Ring((fea::mesh::sections::Ring*) m_mesh->section(i)).exec();
						break;
					case fea::mesh::sections::type::circle:
						Circle((fea::mesh::sections::Circle*) m_mesh->section(i)).exec();
						break;
					case fea::mesh::sections::type::rectangle:
						Rectangle((fea::mesh::sections::Rectangle*) m_mesh->section(i)).exec();
						break;
					default:
						return;
				}
				table_update(i);
			}
			void Sections::slot_index(int i)
			{
				if(i != -1)
				{
					m_ui->table->selectRow(i);
					m_ui->edit_name->setText(m_mesh->section(i)->label());
					m_ui->combo_type->setCurrentIndex(mat::bit_index(unsigned(m_mesh->section(i)->type())));
				}
			}
			void Sections::slot_rebars(void)
			{
				//index
				const unsigned i = (unsigned) m_ui->combo_index->currentIndex();
				//dialog
				Rebars(m_mesh->section(i), nullptr).exec();
				//update
				m_ui->table->item(i, 2)->setText(QString::asprintf("%d", m_mesh->section(i)->rebars()));
			}
			void Sections::slot_remove(void)
			{
				//index
				const unsigned i = (unsigned) m_ui->combo_index->currentIndex();
				//remove section
				m_mesh->model()->mark();
				m_mesh->remove_section(i);
				const unsigned ns = m_mesh->sections().size();
				//set combo
				m_ui->combo_index->removeItem(i);
				m_ui->combo_index->setEnabled(ns != 0);
				for(unsigned j = i; j < ns; j++)
				{
					m_ui->combo_index->setItemText(j, QString::asprintf("%02d", j + 1));
				}
				//set table
				m_ui->table->removeRow(i);
				m_ui->table->setEnabled(ns != 0);
				//set pushs
				m_ui->edit_name->setEnabled(ns != 0);
				m_ui->push_remove->setEnabled(ns != 0);
				m_ui->push_rebars->setEnabled(ns != 0);
			}
			
			//table
			void Sections::table_add(unsigned i) const
			{
				//section
				fea::mesh::sections::Section* section = m_mesh->section(i);
				//data
				const double p[] = {
					section->area(),
					section->inertia_z(),
					section->inertia_y(),
					section->inertia_x(),
					section->elastic_modulus_z(),
					section->elastic_modulus_y(),
					section->plastic_modulus_z(),
					section->plastic_modulus_y(),
					section->radius_of_gyration_z(),
					section->radius_of_gyration_y()
				};
				//type
				QTableWidgetItem* it = new QTableWidgetItem(section->name());
				it->setFlags(it->flags() ^ Qt::ItemIsEditable);
				it->setTextAlignment(Qt::AlignCenter);
				m_ui->table->setItem(i, 0, it);
				//name
				QTableWidgetItem* in = new QTableWidgetItem(section->label());
				in->setFlags(in->flags() ^ Qt::ItemIsEditable);
				in->setTextAlignment(Qt::AlignCenter);
				m_ui->table->setItem(i, 1, in);
				//rebars
				QTableWidgetItem* ir = new QTableWidgetItem(QString::asprintf("%d", section->rebars()));
				ir->setFlags(ir->flags() ^ Qt::ItemIsEditable);
				ir->setTextAlignment(Qt::AlignCenter);
				m_ui->table->setItem(i, 2, ir);
				//items
				for(unsigned j = 0; j < 10; j++)
				{
					QTableWidgetItem* it = new QTableWidgetItem(QString::asprintf("%.2e", p[j]));
					it->setFlags(it->flags() ^ Qt::ItemIsEditable);
					it->setTextAlignment(Qt::AlignCenter);
					m_ui->table->setItem(i, j + 3, it);
				}
			}
			void Sections::table_update(unsigned i) const
			{
				//section
				fea::mesh::sections::Section* section = m_mesh->section(i);
				//data
				const double p[] = {
					section->area(),
					section->inertia_z(),
					section->inertia_y(),
					section->inertia_x(),
					section->elastic_modulus_z(),
					section->elastic_modulus_y(),
					section->plastic_modulus_z(),
					section->plastic_modulus_y(),
					section->radius_of_gyration_z(),
					section->radius_of_gyration_y()
				};
				//items
				m_ui->table->item(i, 1)->setText(section->label());
				m_ui->table->item(i, 2)->setText(QString::asprintf("%d", section->rebars()));
				for(unsigned j = 0; j < 10; j++)
				{
					m_ui->table->item(i, j + 3)->setText(QString::asprintf("%.2e", p[j]));
				}
			}
		}
	}
}
