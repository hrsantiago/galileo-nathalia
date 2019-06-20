#pragma once

#include <QDialog>

namespace Ui
{
	class Rectangle;
}
namespace fea
{
	namespace mesh
	{
		namespace sections
		{
			class Rectangle;
		}
	}
}

namespace gui
{
	namespace mesh
	{
		namespace sections
		{
			class Rectangle : public QDialog
			{
				Q_OBJECT

			public:
				//constructors
				Rectangle(fea::mesh::sections::Rectangle*, QWidget* = nullptr);

				//destructor
				virtual ~Rectangle(void);

			private slots:
				//slots
				void slot_name(void);
				void slot_width(void);
				void slot_height(void);
				void slot_mesh_w(void);
				void slot_mesh_h(void);
				void slot_rebars(void);

			private:
				//members
				Ui::Rectangle* m_ui;
				fea::mesh::sections::Rectangle* m_rectangle;
			};
		}
	}
}
