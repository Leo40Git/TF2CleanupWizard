#include "cleanupwizard.h"

#include "intropage.h"
#include "steampathpage.h"
#include "tf2pathpage.h"
#include "selectuserpage.h"
#include "warningpage.h"
#include "cleanupdialog.h"

#include <QDebug>

CleanupWizard::CleanupWizard(QWidget *parent) : QWizard(parent)
{
    setWindowTitle("TF2 Cleanup Wizard");
    setOption(QWizard::HelpButtonOnRight, false);
    setOption(QWizard::HaveHelpButton, false);
    setWindowFlag(Qt::CustomizeWindowHint);
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    setPixmap(WizardPixmap::LogoPixmap, QPixmap(":/icon.png"));
    setWizardStyle(WizardStyle::ClassicStyle);

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
    dialog->setSteamID(static_cast<quint64>(property("steamID").toULongLong()));
    connect(dialog, &QDialog::finished, this, &CleanupWizard::done);
    dialog->open();
}

quint64 CleanupWizard::steamID() { return m_steamID; }

void CleanupWizard::setSteamID(quint64 steamID) { m_steamID = steamID; }

void CleanupWizard::done(int result)
{
    QWizard::done(result);
}
