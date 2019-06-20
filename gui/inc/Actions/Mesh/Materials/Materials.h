#pragma once

#include <QDialog>

namespace Ui
{
	class Materials;
}
namespace fea
{
	namespace mesh
	{
		class Mesh;
	}
}

namespace gui
{
	namespace mesh
	{
		namespace materials
		{
			class Materials : public QDialog
			{
				Q_OBJECT

			public:
				//constructors
				Materials(fea::mesh::Mesh*, QWidget* = nullptr);

				//destructor
				virtual ~Materials(void);

			private slots:
				//slots
				void slot_add(void);
				void slot_name(void);
				void slot_mass(void);
				void slot_click(int);
				void slot_index(int);
				void slot_remove(void);

			private:
				//misc
				void add_material(unsigned) const;
				void update_material(unsigned) const;
			
				//attributes
				Ui::Materials* m_ui;
				fea::mesh::Mesh* m_mesh;
			};
		}
	}
}
