#include <QApplication>

#include "cleanupwizard.h"

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(resources);

    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    QApplication a(argc, argv);

    CleanupWizard wizard;
    wizard.show();

    return a.exec();
}
