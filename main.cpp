#include <QApplication>
#include <QWizard>

#include "pageids.h"
#include "intropage.h"
#include "selectuserpage.h"
#include "steampathpage.h"
#include "tf2pathpage.h"

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(resources);

    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    QApplication a(argc, argv);

    QWizard wizard;

    wizard.setPage(Page_Intro, new IntroPage);
    wizard.setPage(Page_SteamPath, new SteamPathPage);
    wizard.setPage(Page_TF2Path, new TF2PathPage);
    wizard.setPage(Page_SelectUser, new SelectUserPage);

    wizard.setStartId(Page_Intro);
    wizard.setWindowTitle("TF2 Cleanup Wizard");
    wizard.setOption(QWizard::HelpButtonOnRight, false);
    wizard.setOption(QWizard::HaveHelpButton, false);
    wizard.setWindowFlag(Qt::CustomizeWindowHint);
    wizard.setWindowFlag(Qt::WindowContextHelpButtonHint, false);

    wizard.show();

    return a.exec();
}
