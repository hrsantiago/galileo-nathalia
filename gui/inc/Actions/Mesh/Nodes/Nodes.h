#pragma once

#include <QDialog>

namespace Ui
{
	class Nodes;
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
		namespace nodes
		{
			class Nodes : public QDialog
			{
				Q_OBJECT

			public:
				//constructors
				Nodes(fea::mesh::Mesh*, gui::canvas::Model*, QWidget* = nullptr);

				//destructor
				virtual ~Nodes(void);

			private slots:
				//slots
				void slot_add(void);
				void slot_edit(void);
				void slot_click(int);
				void slot_index(int);
				void slot_remove(void);
				void slot_table(int, int);

			private:
				//members
				Ui::Nodes* m_ui;
				fea::mesh::Mesh* m_mesh;
				gui::canvas::Model* m_canvas;
			};
		}
	}
}
