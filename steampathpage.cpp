#include "steampathpage.h"

#include <QPushButton>
#include <QDir>
#include <QFileDialog>
#include <QGridLayout>
#include <QSettings>
#include <QDesktopServices>
#include <QUrl>

SteamPathPage::SteamPathPage(QWidget *parent) : QWizardPage(parent)
{
    setTitle("Select Steam Path");
    setSubTitle("Please select your Steam installation directory.");

    cbDirectory = new QComboBox(this);
    cbDirectory->setEditable(true);
    cbDirectory->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    registerField("pathSteam*", cbDirectory, "currentText", SIGNAL(currentTextChanged(QString)));

    QPushButton *button = new QPushButton("&Browse...", this);
    connect(button, &QPushButton::clicked, this, &SteamPathPage::browse);

    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget(cbDirectory, 1, 0);
    layout->addWidget(button, 1, 1);

    setLayout(layout);
}

void SteamPathPage::initializePage()
{
    QString defDir = "";
    // try to read install path from registry
    QSettings registry("HKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432Node\\Valve\\Steam", QSettings::NativeFormat);
    defDir = registry.value("InstallPath").toString();
    if (defDir.isEmpty()) {
        // fallback (32-bit system)
        QSettings registry2("HKEY_LOCAL_MACHINE\\SOFTWARE\\Valve\\Steam", QSettings::NativeFormat);
        defDir = registry2.value("InstallPath").toString();
    }
    if (!defDir.isEmpty()) {
        defDir = defDir.replace('\\', '/');
        cbDirectory->clear();
        cbDirectory->addItem(defDir);
    }
}

void SteamPathPage::browse()
{
    QString directory =
            QFileDialog::getExistingDirectory(this, "", cbDirectory->currentText());

    if (!directory.isEmpty()) {
        if (cbDirectory->findText(directory) == -1)
            cbDirectory->addItem(directory);
        cbDirectory->setCurrentIndex(cbDirectory->findText(directory));
    }
}

bool SteamPathPage::isComplete() const
{
    QString chk = cbDirectory->currentText();
    if (chk.isEmpty())
        return false;
    if (!QDir(chk).exists())
        return false;
    if (!QDir(chk + "/userdata").exists())
        return false;
    return QDir(chk + "/config").exists();
}
