//qt
#include <QUrl>
#include <QDesktopServices>

//gui
#include "Actions/Help/About.h"

//ui
#include "ui_About.h"

namespace gui
{
	namespace help
	{
		//constructors
		About::About(QWidget* parent) : QDialog(parent), m_ui(new Ui::About)
		{
			m_ui->setupUi(this);
		}
		
		//destructor
		About::~About(void)
		{
			delete m_ui;
		}
	}
}
