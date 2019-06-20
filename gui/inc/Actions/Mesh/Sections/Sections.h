#pragma once

#include <QDialog>

namespace Ui
{
	class Sections;
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
		namespace sections
		{
			class Sections : public QDialog
			{
				Q_OBJECT

			public:
				//constructors
				Sections(fea::mesh::Mesh*, QWidget* = nullptr);

				//destructor
				virtual ~Sections(void);

			private slots:
				//slots
				void slot_add(void);
				void slot_name(void);
				void slot_click(int);
				void slot_index(int);
				void slot_rebars(void);
				void slot_remove(void);

			private:
				//misc
				void add_section(unsigned) const;
				void update_section(unsigned) const;
			
				//attributes
				Ui::Sections* m_ui;
				fea::mesh::Mesh* m_mesh;
			};
		}
	}
}
