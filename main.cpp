#include <QApplication>
#include <QWizard>

#include "intropage.h"

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(resources);

    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    QApplication a(argc, argv);

    QWizard wizard;

    wizard.addPage(new IntroPage);

    wizard.setWindowTitle("TF2 Cleanup Wizard");

    wizard.show();

    return a.exec();
}
