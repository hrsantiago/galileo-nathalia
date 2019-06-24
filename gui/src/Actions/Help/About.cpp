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
			//set ui
			m_ui->setupUi(this);
			m_ui->logo->setPixmap(QPixmap("/usr/share/galileo/rsc/galileo.jpg"));
			//connect
			QObject::connect(m_ui->logo, SIGNAL(clicked(void)), this, SLOT(slot_logo(void)));
		}
		
		//destructor
		About::~About(void)
		{
			delete m_ui;
		}
		
		//slots
		void About::slot_logo(void) const
		{
			QDesktopServices::openUrl(QUrl("https://en.wikipedia.org/wiki/Galileo_Galilei"));
		}
	}
}
