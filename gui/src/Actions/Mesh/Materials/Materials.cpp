//std
#include <cmath>

//qt
#include <QDoubleValidator>

//mat
#include "misc/util.h"

//fea
#include "Models/Model.h"

#include "Mesh/Mesh.h"
#include "Mesh/Materials/Materials.h"

//gui
#include "Actions/Mesh/Materials/Heat.h"
#include "Actions/Mesh/Materials/Steel.h"
#include "Actions/Mesh/Materials/Concrete.h"
#include "Actions/Mesh/Materials/Materials.h"

//ui
#include "ui_Materials.h"

namespace gui
{
	namespace mesh
	{
		namespace materials
		{
			//constructors
			Materials::Materials(fea::mesh::Mesh* mesh, QWidget* parent) : QDialog(parent), m_ui(new Ui::Materials), m_mesh(mesh)
			{
				//set ui
				m_ui->setupUi(this);
				const unsigned nm = m_mesh->materials();
				//set combo type
				for(unsigned i = 1; i < unsigned(fea::mesh::materials::type::last); i <<= 1)
				{
					m_ui->combo_type->addItem(fea::mesh::materials::Material::name(fea::mesh::materials::type(i)));
				}
				//set combo index
				if(nm == 0)
				{
					m_ui->table->setEnabled(false);
					m_ui->edit_name->setEnabled(false);
					m_ui->edit_mass->setEnabled(false);
					m_ui->combo_index->setEnabled(false);
					m_ui->button_remove->setEnabled(false);
				}
				else
				{
					for (unsigned i = 0; i < nm; i++)
					{
						m_ui->combo_index->addItem(QString::asprintf("%02d", i + 1));
					}
				}
				//set edits
				m_ui->edit_name->setText(nm == 0 ? "" : m_mesh->material(0)->label());
				m_ui->edit_mass->setValidator(new QDoubleValidator(0, std::numeric_limits<double>::max(), 3));
				m_ui->edit_mass->setText(nm == 0 ? "" : QString::asprintf("%.2e", m_mesh->material(0)->specific_mass()));
				//set table
				m_ui->table->setRowCount(nm);
				m_ui->table->setColumnCount(3);
				m_ui->table->setHorizontalHeaderItem(0, new QTableWidgetItem("Type"));
				m_ui->table->setHorizontalHeaderItem(1, new QTableWidgetItem("Name"));
				m_ui->table->setHorizontalHeaderItem(2, new QTableWidgetItem("Specific Mass"));
				m_ui->table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
				for(unsigned i = 0; i < nm; i++)
				{
					add_material(i);
				}
				//slots
				QObject::connect(m_ui->button_add, SIGNAL(clicked(bool)), SLOT(slot_add(void)));
				QObject::connect(m_ui->table, SIGNAL(cellClicked(int, int)), SLOT(slot_click(int)));
				QObject::connect(m_ui->button_remove, SIGNAL(clicked(bool)), SLOT(slot_remove(void)));
				QObject::connect(m_ui->edit_name, SIGNAL(editingFinished(void)), SLOT(slot_name(void)));
				QObject::connect(m_ui->edit_mass, SIGNAL(editingFinished(void)), SLOT(slot_mass(void)));
				QObject::connect(m_ui->combo_index, SIGNAL(currentIndexChanged(int)), SLOT(slot_index(int)));
			}

			//destructors
			Materials::~Materials(void)
			{
				delete m_ui;
			}

			//slots
			void Materials::slot_add(void)
			{
				//add material
				const unsigned nm = m_mesh->materials() + 1;
				const unsigned st = m_ui->combo_type->currentIndex();
				fea::mesh::materials::Material* material = m_mesh->add_material(fea::mesh::materials::type(1 << st));
				//set select
				m_ui->combo_index->setEnabled(true);
				m_ui->combo_index->addItem(QString::asprintf("%02d", nm));
				m_ui->combo_index->setCurrentIndex(nm - 1);
				//set edits
				m_ui->edit_mass->setEnabled(true);
				m_ui->edit_name->setEnabled(true);
				m_ui->edit_name->setText(material->label());
				m_ui->edit_mass->setText(QString::asprintf("%.2e", material->specific_mass()));
				//set remove
				m_ui->button_remove->setEnabled(true);
				//set table
				m_ui->table->setRowCount(nm);
				m_ui->table->setEnabled(true);
				//add section
				add_material(nm - 1);
			}
			void Materials::slot_name(void)
			{
				QString text = m_ui->edit_name->text();
				const unsigned i = m_ui->combo_index->currentIndex();
				m_mesh->material(i)->label(text.remove(' ').toStdString().c_str());
				m_ui->table->item(i, 1)->setText(text);
				m_ui->edit_name->setText(text);
			}
			void Materials::slot_mass(void)
			{
				//data
				const double m = m_ui->edit_mass->text().toDouble();
				const unsigned i = m_ui->combo_index->currentIndex();
				//material
				m_mesh->material(i)->specific_mass(m);
				//edit and table
				m_ui->edit_mass->setText(QString::asprintf("%.2e", m));
				m_ui->table->item(i, 2)->setText(QString::asprintf("%.2e", m));
			}
			void Materials::slot_click(int i)
			{
				switch(m_mesh->material(i)->type())
				{
					case fea::mesh::materials::type::heat:
						Heat((fea::mesh::materials::Heat*) m_mesh->material(i), nullptr).exec();
						break;
					case fea::mesh::materials::type::steel:
						Steel((fea::mesh::materials::Steel*) m_mesh->material(i), nullptr).exec();
						break;
					case fea::mesh::materials::type::concrete:
						Concrete((fea::mesh::materials::Concrete*) m_mesh->material(i), nullptr).exec();
						break;
				}
				update_material(i);
			}
			void Materials::slot_index(int i)
			{
				if(i != -1)
				{
					m_ui->table->selectRow(i);
					fea::mesh::materials::Material* material = m_mesh->material(i);
					m_ui->edit_name->setText(material->label());
					m_ui->combo_type->setCurrentIndex(mat::log2(unsigned(material->type())));
					m_ui->edit_mass->setText(QString::asprintf("%.2e", material->specific_mass()));
				}
			}
			void Materials::slot_remove(void)
			{
				//indes
				const unsigned i = (unsigned) m_ui->combo_index->currentIndex();
				//remove material
				m_mesh->remove_material(i);
				const unsigned nm = m_mesh->materials();
				//set combo
				m_ui->combo_index->removeItem(i);
				m_ui->combo_index->setEnabled(nm != 0);
				for(unsigned j = i; j < nm; j++)
				{
					m_ui->combo_index->setItemText(j, QString::asprintf("%02d", j + 1));
				}
				//set table
				m_ui->table->removeRow(i);
				m_ui->table->setEnabled(nm != 0);
				//set edits
				m_ui->edit_name->setEnabled(nm != 0);
				m_ui->edit_mass->setEnabled(nm != 0);
				//set buttons
				m_ui->button_remove->setEnabled(nm != 0);
			}
			
			//add
			void Materials::add_material(unsigned i) const
			{
				//material
				fea::mesh::materials::Material* material = m_mesh->material(i);
				//items
				QTableWidgetItem* it = new QTableWidgetItem(material->name());
				QTableWidgetItem* in = new QTableWidgetItem(material->label());
				QTableWidgetItem* im = new QTableWidgetItem(QString::asprintf("%.2e", material->specific_mass()));
				//table
				m_ui->table->setItem(i, 0, it);
				m_ui->table->setItem(i, 1, in);
				m_ui->table->setItem(i, 2, im);
				it->setTextAlignment(Qt::AlignCenter);
				in->setTextAlignment(Qt::AlignCenter);
				im->setTextAlignment(Qt::AlignCenter);
				it->setFlags(it->flags() ^ Qt::ItemIsEditable);
				in->setFlags(in->flags() ^ Qt::ItemIsEditable);
				im->setFlags(im->flags() ^ Qt::ItemIsEditable);
			}
			void Materials::update_material(unsigned i) const
			{
				//material
				fea::mesh::materials::Material* material = m_mesh->material(i);
				//combo
				m_ui->combo_index->setCurrentIndex(i);
				//edits
				m_ui->edit_name->setText(material->label());
				m_ui->edit_mass->setText(QString::asprintf("%.2e", material->specific_mass()));
				//table
				m_ui->table->item(i, 1)->setText(material->label());
				m_ui->table->item(i, 2)->setText(QString::asprintf("%.2e", material->specific_mass()));
			}
		}
	}
}
