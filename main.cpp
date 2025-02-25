#include "opera_notes.h"

#include <QtGui>
#include <QApplication>
#include <string>
#include <iostream>

int main(int argc, char *argv[]) {
    QCoreApplication::setApplicationName(APP_NAME);
    QCoreApplication::setApplicationVersion(APP_VERSION);
    QCoreApplication::setOrganizationName(APP_COMPANY);
    QCoreApplication::setOrganizationDomain(APP_COMPANY_DOMAIN);

    QApplication a(argc, argv);
    opera_notes w;
    w.show();
    return a.exec();
}
