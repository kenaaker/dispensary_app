#include "meds_dispensary.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Meds_dispensary w;
    w.show();

    return a.exec();
}
