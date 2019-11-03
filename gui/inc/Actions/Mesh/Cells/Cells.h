#pragma once

#include <QDialog>

namespace Ui
{
	class Cells;
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
		namespace cells
		{
			class Cells : public QDialog
			{
				Q_OBJECT

			public:
				//constructors
				Cells(fea::mesh::Mesh*, QWidget* = nullptr);

				//destructor
				virtual ~Cells(void);

			private slots:
				//slots
				void slot_add(void);
				void slot_name(void);
				void slot_click(int);
				void slot_index(int);
				void slot_remove(void);

			private:
				//table
				void table_add(unsigned) const;
				void table_update(unsigned) const;
				
				//attributes
				Ui::Cells* m_ui;
				fea::mesh::Mesh* m_mesh;
			};
		}
	}
}
