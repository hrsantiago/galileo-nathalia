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
			m_ui->logo_puc->setPixmap(QPixmap("/usr/share/galileo/rsc/puc.png"));
			m_ui->logo_ulb->setPixmap(QPixmap("/usr/share/galileo/rsc/ulb.png"));
			m_ui->logo_ufop->setPixmap(QPixmap("/usr/share/galileo/rsc/ufop.png"));
			m_ui->logo_galileo->setPixmap(QPixmap("/usr/share/galileo/rsc/galileo.png"));
			//connect
			QObject::connect(m_ui->logo_puc, SIGNAL(clicked(void)), this, SLOT(slot_puc(void)));
			QObject::connect(m_ui->logo_ulb, SIGNAL(clicked(void)), this, SLOT(slot_ulb(void)));
			QObject::connect(m_ui->logo_ufop, SIGNAL(clicked(void)), this, SLOT(slot_ufop(void)));
			QObject::connect(m_ui->logo_galileo, SIGNAL(clicked(void)), this, SLOT(slot_galileo(void)));
		}
		
		//destructor
		About::~About(void)
		{
			delete m_ui;
		}
		
		//slots
		void About::slot_puc(void) const
		{
			QDesktopServices::openUrl(QUrl("http://www.civ.puc-rio.br/"));
		}
		void About::slot_ulb(void) const
		{
			QDesktopServices::openUrl(QUrl("https://batir.ulb.ac.be/"));
		}
		void About::slot_ufop(void) const
		{
			QDesktopServices::openUrl(QUrl("http://www.em.ufop.br/"));
		}
		void About::slot_galileo(void) const
		{
			QDesktopServices::openUrl(QUrl("https://en.wikipedia.org/wiki/Galileo_Galilei"));
		}
	}
}
