#pragma once

#include <QDialog>

namespace Ui
{
	class Volume;
}
namespace fea
{
	namespace mesh
	{
		namespace cells
		{
			class Volume;
		}
	}
}

namespace gui
{
	namespace mesh
	{
		namespace cells
		{
			class Volume : public QDialog
			{
				Q_OBJECT

			public:
				//constructors
				Volume(fea::mesh::cells::Volume*, QWidget* = nullptr);

				//destructor
				virtual ~Volume(void);

			private:
				//members
				Ui::Volume* m_ui;
				fea::mesh::cells::Volume* m_volume;
			};
		}
	}
}
