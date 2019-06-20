#pragma once

#include <QDialog>

namespace Ui
{
	class Box;
}
namespace fea
{
	namespace mesh
	{
		namespace sections
		{
			class Box;
		}
	}
}

namespace gui
{
	namespace mesh
	{
		namespace sections
		{
			class Box : public QDialog
			{
				Q_OBJECT

			public:
				//constructors
				Box(fea::mesh::sections::Box*, QWidget* = nullptr);

				//destructor
				virtual ~Box(void);

			private slots:
				//slots
				void slot_name(void);
				void slot_width(void);
				void slot_thick(void);
				void slot_height(void);
				void slot_mesh_w(void);
				void slot_mesh_h(void);
				void slot_mesh_t(void);
				void slot_rebars(void);

			private:
				//members
				Ui::Box* m_ui;
				fea::mesh::sections::Box* m_box;
			};
		}
	}
}
