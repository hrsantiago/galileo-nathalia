#pragma once

#include <QDialog>

namespace Ui
{
	class Elements;
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
	namespace canvas
	{
		class Model;
	}
}

namespace gui
{
	namespace mesh
	{
		namespace elements
		{
			class Elements : public QDialog
			{
				Q_OBJECT

			public:
				//constructors
				Elements(fea::mesh::Mesh*, gui::canvas::Model*, QWidget* = nullptr);

				//destructor
				virtual ~Elements(void);

			private slots:
				//slots
				void slot_add(void);
				void slot_click(int);
				void slot_index(int);
				void slot_remove(void);

			private:
				//table
				void table_add(unsigned) const;
				void table_update(unsigned) const;
			
				//attributes
				Ui::Elements* m_ui;
				fea::mesh::Mesh* m_mesh;
				gui::canvas::Model* m_canvas;
			};
		}
	}
}
