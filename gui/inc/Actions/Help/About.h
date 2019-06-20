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
			
		private slots:
			void slot_puc(void) const;
			void slot_ulb(void) const;
			void slot_ufop(void) const;
			void slot_galileo(void) const;
		
		private:
			//attributes
			Ui::About* m_ui;
		};
	}
}
