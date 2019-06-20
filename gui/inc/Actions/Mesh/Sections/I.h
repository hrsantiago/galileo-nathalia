#pragma once

#include <QDialog>

namespace Ui
{
	class I;
}
namespace fea
{
	namespace mesh
	{
		namespace sections
		{
			class I;
		}
	}
}

namespace gui
{
	namespace mesh
	{
		namespace sections
		{
			class I : public QDialog
			{
				Q_OBJECT

			public:
				//constructors
				I(fea::mesh::sections::I*, QWidget* = nullptr);

				//destructor
				virtual ~I(void);

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
				Ui::I* m_ui;
				fea::mesh::sections::I* m_i;
			};
		}
	}
}
