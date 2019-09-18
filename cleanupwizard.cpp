#include "cleanupwizard.h"

#include "intropage.h"
#include "steampathpage.h"
#include "tf2pathpage.h"
#include "selectuserpage.h"
#include "warningpage.h"
#include "cleanupdialog.h"

CleanupWizard::CleanupWizard(QWidget *parent) : QWizard(parent)
{
    setWindowTitle("TF2 Cleanup Wizard");
    setOption(QWizard::HelpButtonOnRight, false);
    setOption(QWizard::HaveHelpButton, false);
    setWindowFlag(Qt::CustomizeWindowHint);
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);

    addPage(new IntroPage);
    addPage(new SteamPathPage);
    addPage(new TF2PathPage);
    addPage(new SelectUserPage);
    addPage(new WarningPage);
}

void CleanupWizard::accept()
{
    CleanupDialog *dialog = new CleanupDialog(this);
    dialog->setPathSteam(field("pathSteam").toString());
    dialog->setPathTF2(field("pathTF2").toString());
    dialog->setSteamID(static_cast<quint64>(field("pathSteam").toULongLong()));
    connect(dialog, &QDialog::finished, this, &QWizard::accept);
    dialog->open();
}
