#pragma once

#include <QDialog>

namespace Ui
{
	class T;
}
namespace fea
{
	namespace mesh
	{
		namespace sections
		{
			class T;
		}
	}
}

namespace gui
{
	namespace mesh
	{
		namespace sections
		{
			class T : public QDialog
			{
				Q_OBJECT

			public:
				//constructors
				T(fea::mesh::sections::T*, QWidget* = nullptr);

				//destructor
				virtual ~T(void);

			private slots:
				//slots
				void slot_name(void);
				void slot_rebars(void);
				void slot_mesh_wh(void);
				void slot_mesh_fw(void);
				void slot_mesh_wt(void);
				void slot_mesh_ft(void);
				void slot_geom_wh(void);
				void slot_geom_wt(void);
				void slot_geom_fw(void);
				void slot_geom_ft(void);

			private:
				//members
				Ui::T* m_ui;
				fea::mesh::sections::T* m_t;
			};
		}
	}
}
