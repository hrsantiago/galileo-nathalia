#pragma once

#include <QDialog>

namespace Ui
{
	class Heat;
}
namespace fea
{
	namespace mesh
	{
		namespace materials
		{
			class Heat;
		}
	}
}

namespace gui
{
	namespace mesh
	{
		namespace materials
		{
			class Heat : public QDialog
			{
				Q_OBJECT

			public:
				//constructors
				Heat(fea::mesh::materials::Heat*, QWidget* = nullptr);

				//destructor
				virtual ~Heat(void);

			private slots:
				//slots
				void slot_name(void);
				void slot_mass(void);
				void slot_capacity(void);
				void slot_condutivity(void);

			private:
				//members
				Ui::Heat* m_ui;
				fea::mesh::materials::Heat* m_heat;
			};
		}
	}
}
