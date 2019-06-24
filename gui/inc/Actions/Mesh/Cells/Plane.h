#pragma once

#include <QDialog>

namespace Ui
{
	class Plane;
}
namespace fea
{
	namespace mesh
	{
		namespace cells
		{
			class Plane;
		}
	}
}

namespace gui
{
	namespace mesh
	{
		namespace cells
		{
			class Plane : public QDialog
			{
				Q_OBJECT

			public:
				//constructors
				Plane(fea::mesh::cells::Plane*, QWidget* = nullptr);

				//destructor
				virtual ~Plane(void);

			private slots:
				//slots
				void slot_edit(void);

			private:
				//members
				Ui::Plane* m_ui;
				fea::mesh::cells::Plane* m_plane;
			};
		}
	}
}
