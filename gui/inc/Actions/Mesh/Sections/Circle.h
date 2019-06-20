#pragma once

#include <QDialog>

namespace Ui
{
	class Circle;
}
namespace fea
{
	namespace mesh
	{
		namespace sections
		{
			class Circle;
		}
	}
}

namespace gui
{
	namespace mesh
	{
		namespace sections
		{
			class Circle : public QDialog
			{
				Q_OBJECT

			public:
				//constructors
				Circle(fea::mesh::sections::Circle*, QWidget* = nullptr);

				//destructor
				virtual ~Circle(void);

			private slots:
				//slots
				void slot_name(void);
				void slot_mesh_r(void);
				void slot_mesh_t(void);
				void slot_rebars(void);
				void slot_diameter(void);

			private:
				//members
				Ui::Circle* m_ui;
				fea::mesh::sections::Circle* m_circle;
			};
		}
	}
}
