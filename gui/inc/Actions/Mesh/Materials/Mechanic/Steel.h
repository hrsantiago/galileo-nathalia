#pragma once

#include <QDialog>

namespace Ui
{
	class Steel;
}
namespace fea
{
	namespace mesh
	{
		namespace materials
		{
			class Steel;
		}
	}
}

namespace gui
{
	namespace mesh
	{
		namespace materials
		{
			class Steel : public QDialog
			{
				Q_OBJECT

			public:
				//constructors
				Steel(fea::mesh::materials::Steel*, QWidget* = nullptr);

				//destructor
				virtual ~Steel(void);

			private slots:
				//slots
				void slot_name(void);
				void slot_mass(void);
				void slot_yield(void);
				void slot_strain(void);
				void slot_stress(void);
				void slot_poisson(void);
				void slot_elastic(void);

			private:
				//misc
				void update_canvas(void) const;
				
				//members
				Ui::Steel* m_ui;
				fea::mesh::materials::Steel* m_steel;
			};
		}
	}
}
