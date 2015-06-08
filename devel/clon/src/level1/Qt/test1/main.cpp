/* test1.cpp */

#include <QtGui>
#include "Test1.h"

//! [main function]
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Ui_TabWidget abc;
    abc.show();
    return app.exec();
}
//! [main function]
