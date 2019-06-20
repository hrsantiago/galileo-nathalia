#pragma once

#include <QDialog>

namespace Ui
{
	class Concrete;
}
namespace fea
{
	namespace mesh
	{
		namespace materials
		{
			class Concrete;
		}
	}
}

namespace gui
{
	namespace mesh
	{
		namespace materials
		{
			class Concrete : public QDialog
			{
				Q_OBJECT

			public:
				//constructors
				Concrete(fea::mesh::materials::Concrete*, QWidget* = nullptr);

				//destructor
				virtual ~Concrete(void);

			private slots:
				//slots
				void slot_name(void);
				void slot_mass(void);
				void slot_strain(void);
				void slot_elastic(void);
				void slot_tension(void);
				void slot_biaxial(void);
				void slot_softening(void);
				void slot_compression(void);

			private:
				//misc
				void update_canvas(void) const;
				double return_mapping(double, double&) const;
				
				//members
				Ui::Concrete* m_ui;
				fea::mesh::materials::Concrete* m_concrete;
			};
		}
	}
}
