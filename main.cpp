#include "meds_dispensary.h"
#include <QApplication>
#include <QtCore/qdebug.h>
#include "dispensary_sslsock.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Meds_dispensary w;

    w.show();

    return a.exec();
}
