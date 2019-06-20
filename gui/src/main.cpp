//et
#include <QApplication>

//gui
#include "Galileo/Galileo.h"

int main(int argc, char *argv[])
{
	//application
    QApplication a(argc, argv);
    //window
    gui::Galileo w;
    w.show();
	//execute
    return a.exec();
}
