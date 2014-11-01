#include "v800main.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    v800main w;
    w.show();

    return a.exec();
}
