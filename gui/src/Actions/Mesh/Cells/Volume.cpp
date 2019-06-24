//fea
#include "Mesh/Mesh.h"
#include "Mesh/Cells/Volume/Volume.h"

//gui
#include "Actions/Mesh/Cells/Volume.h"

//ui
#include "ui_Volume.h"

namespace gui
{
	namespace mesh
	{
		namespace cells
		{
			//constructors
			Volume::Volume(fea::mesh::cells::Volume* volume, QWidget* parent) : QDialog(parent), m_ui(new Ui::Volume), m_volume(volume)
			{
				//set ui
				m_ui->setupUi(this);
				setWindowTitle(QString::asprintf("%s - Index: %02d - Name: %s", m_volume->name(), m_volume->index() + 1, m_volume->label()));
			}

			//destructors
			Volume::~Volume(void)
			{
				delete m_ui;
			}
		}
	}
}
