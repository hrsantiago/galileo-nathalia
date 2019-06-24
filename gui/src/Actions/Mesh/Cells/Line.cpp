//fea
#include "Model/Model.h"

#include "Mesh/Mesh.h"
#include "Mesh/Cells/Line/Line.h"
#include "Mesh/Sections/Section.h"

//gui
#include "Actions/Mesh/Cells/Line.h"

//ui
#include "ui_Line.h"

namespace gui
{
	namespace mesh
	{
		namespace cells
		{
			//constructors
			Line::Line(fea::mesh::cells::Line* line, QWidget* parent) : QDialog(parent), m_ui(new Ui::Line), m_line(line)
			{
				//set ui
				m_ui->setupUi(this);
				setWindowTitle(QString::asprintf("%s - Index: %02d - Name: %s", m_line->name(), m_line->index() + 1, m_line->label()));
				//set combo
				for(unsigned i = 0; i < line->mesh()->sections(); i++)
				{
					m_ui->combo->addItem(QString::asprintf("%02d", i + 1));
				}
				m_ui->combo->setCurrentIndex(line->index_section());
				//set labels
				m_ui->label_name->setText(m_line->section()->label());
				m_ui->label_type->setText(fea::mesh::sections::Section::name(m_line->section()->type()));
				//slots
				QObject::connect(m_ui->combo, SIGNAL(currentIndexChanged(int)), SLOT(slot_combo(void)));
			}

			//destructors
			Line::~Line(void)
			{
				delete m_ui;
			}

			//slots
			void Line::slot_combo(void)
			{
				//set line
				m_line->mesh()->model()->mark();
				m_line->section(m_ui->combo->currentIndex());
				//set labels
				m_ui->label_name->setText(m_line->section()->label());
				m_ui->label_type->setText(fea::mesh::sections::Section::name(m_line->section()->type()));
			}
		}
	}
}
