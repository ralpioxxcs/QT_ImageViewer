#include "zoomDialog.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ZoomDialog w;
    w.show();
    return a.exec();
}
