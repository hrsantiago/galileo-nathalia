//gui
#include "Actions/Help/Index.h"

//ui
#include "ui_Index.h"

namespace gui
{
	namespace help
	{
		//constructors
		Index::Index(QWidget* parent) : QDialog(parent), m_ui(new Ui::Index)
		{
			//set ui
			m_ui->setupUi(this);
		}
		
		//destructor
		Index::~Index(void)
		{
			delete m_ui;
		}
	}
}
