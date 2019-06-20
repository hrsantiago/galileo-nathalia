//qt
#include <QMouseEvent>

//gui
#include "Util/Click_Label.h"

namespace gui
{
	namespace util
	{
		//constructors
		Click_Label::Click_Label(QWidget* parent) : QLabel(parent) 
		{
			return;
		}

		//destructor
		Click_Label::~Click_Label(void)
		{
			return;
		}

		//events
		void Click_Label::mousePressEvent(QMouseEvent* e) 
		{
			if(e->button() == Qt::LeftButton)
			{
				emit clicked();
			}
		}
	}
}
