#pragma once

#include <QDialog>

namespace Ui
{
	class Index;
}

namespace gui
{
	namespace help
	{
		class Index : public QDialog
		{
			Q_OBJECT

		public:
			//constructors
			Index(QWidget* = nullptr);
	
			//destructor
			virtual ~Index(void);
			
		private:
			//attributes
			Ui::Index* m_ui;
		};
	}
}
