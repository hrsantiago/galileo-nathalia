#pragma once

#include <QDialog>

namespace Ui
{
	class About;
}

namespace gui
{
	namespace help
	{
		class About : public QDialog
		{
			Q_OBJECT

		public:
			//constructors
			About(QWidget* = nullptr);
	
			//destructor
			virtual ~About(void);
			
		private:
			//attributes
			Ui::About* m_ui;
		};
	}
}
