#pragma once

#include <QLabel>

namespace gui
{
	namespace util
	{
		class Click_Label : public QLabel 
		{ 
			Q_OBJECT 

		public:
		//constructors
			Click_Label(QWidget* = nullptr);
			
			//destructor
			~Click_Label(void);

		signals:
			//signals
			void clicked(void);

		protected:
			//events
			void mousePressEvent(QMouseEvent* event);
		};
	}
}
