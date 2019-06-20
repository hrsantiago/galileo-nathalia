#pragma once

#include <QDialog>

namespace Ui
{
	class Rebars;
}
namespace fea
{
	namespace mesh
	{
		namespace sections
		{
			class Section;
		}
	}
}
namespace gui
{
	namespace canvas
	{
		class Section;
	}
}

namespace gui
{
	namespace mesh
	{
		namespace sections
		{
			class Rebars : public QDialog
			{
				Q_OBJECT

			public:
				//constructors
				Rebars(fea::mesh::sections::Section*, gui::canvas::Section*, QWidget* = nullptr);

				//destructor
				virtual ~Rebars(void);

			private slots:
				//slots
				void slot_add(void);
				void slot_remove(void);
				void slot_table(int, int);

			private:
				//misc
				void add_rebar(unsigned i) const;
				
				//members
				Ui::Rebars* m_ui;
				gui::canvas::Section* m_canvas;
				fea::mesh::sections::Section* m_section;
			};
		}
	}
}
