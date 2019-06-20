#pragma once

#include <QDialog>

namespace Ui
{
	class Ring;
}
namespace fea
{
	namespace mesh
	{
		namespace sections
		{
			class Ring;
		}
	}
}

namespace gui
{
	namespace mesh
	{
		namespace sections
		{
			class Ring : public QDialog
			{
				Q_OBJECT

			public:
				//constructors
				Ring(fea::mesh::sections::Ring*, QWidget* = nullptr);

				//destructor
				virtual ~Ring(void);

			private slots:
				//slots
				void slot_name(void);
				void slot_thick(void);
				void slot_mesh_r(void);
				void slot_mesh_t(void);
				void slot_rebars(void);
				void slot_diameter(void);

			private:
				//members
				Ui::Ring* m_ui;
				fea::mesh::sections::Ring* m_ring;
			};
		}
	}
}
