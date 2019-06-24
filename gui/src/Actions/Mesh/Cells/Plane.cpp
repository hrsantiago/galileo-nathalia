//qt
#include <QDoubleValidator>

//fea
#include "Model/Model.h"

#include "Mesh/Mesh.h"
#include "Mesh/Cells/Plane/Plane.h"

//gui
#include "Actions/Mesh/Cells/Plane.h"

//ui
#include "ui_Plane.h"

namespace gui
{
	namespace mesh
	{
		namespace cells
		{
			//constructors
			Plane::Plane(fea::mesh::cells::Plane* plane, QWidget* parent) : QDialog(parent), m_ui(new Ui::Plane), m_plane(plane)
			{
				//set ui
				m_ui->setupUi(this);
				setWindowTitle(QString::asprintf("%s - Index: %02d - Name: %s", m_plane->name(), m_plane->index() + 1, m_plane->label()));
				//set edit
				m_ui->edit->setText(QString::asprintf("%.2lf", m_plane->thickness()));
				m_ui->edit->setValidator(new QDoubleValidator(0, std::numeric_limits<double>::max(), 3));
				//slots
				QObject::connect(m_ui->edit, SIGNAL(editingFinished(void)), SLOT(slot_edit(void)));
			}

			//destructors
			Plane::~Plane(void)
			{
				delete m_ui;
			}

			//slots
			void Plane::slot_edit(void)
			{
				m_plane->mesh()->model()->mark();
				m_plane->thickness(m_ui->edit->text().toDouble());
			}
		}
	}
}
