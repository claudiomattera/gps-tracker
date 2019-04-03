#include "MainWindow.h"
#include <QApplication>

#include <QMetaType>
#include <QList>
#include <QPointF>

#include "version.h"

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);

    qRegisterMetaTypeStreamOperators<QList<QPointF> >("QList<QPointF>");

    application.setApplicationName("GPS Tracker");
    application.setApplicationVersion(GIT_VERSION);
    application.setOrganizationName("Claudio Mattera");
    application.setOrganizationDomain("https://gitlab.org/claudio.mattera/");

    MainWindow mainWindow;
    mainWindow.show();

    return application.exec();
}
