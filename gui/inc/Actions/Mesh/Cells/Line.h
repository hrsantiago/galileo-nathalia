#pragma once

#include <QDialog>

namespace Ui
{
	class Line;
}
namespace fea
{
	namespace mesh
	{
		namespace cells
		{
			class Line;
		}
	}
}

namespace gui
{
	namespace mesh
	{
		namespace cells
		{
			class Line : public QDialog
			{
				Q_OBJECT

			public:
				//constructors
				Line(fea::mesh::cells::Line*, QWidget* = nullptr);

				//destructor
				virtual ~Line(void);

			private slots:
				//slots
				void slot_combo(void);

			private:
				//members
				Ui::Line* m_ui;
				fea::mesh::cells::Line* m_line;
			};
		}
	}
}
