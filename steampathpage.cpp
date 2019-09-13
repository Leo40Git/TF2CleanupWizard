#include "steampathpage.h"

#include <QLabel>
#include <QPushButton>
#include <QDir>
#include <QFileDialog>
#include <QGridLayout>
#include <QSettings>

SteamPathPage::SteamPathPage(QWidget *parent) : QWizardPage(parent)
{
    setTitle("Select Steam Path");

    QLabel *label = new QLabel("Please select your Steam installation directory:", this);

    cbDirectory = new QComboBox(this);
    cbDirectory->setEditable(true);
    cbDirectory->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    registerField("pathSteam*", cbDirectory, "currentText", SIGNAL(currentTextChanged(QString)));

    QPushButton *button = new QPushButton("&Browse...", this);
    connect(button, &QPushButton::clicked, this, &SteamPathPage::browse);

    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget(label, 0, 0);
    layout->addWidget(cbDirectory, 1, 0);
    layout->addWidget(button, 1, 1);

    setLayout(layout);
}

void SteamPathPage::initializePage()
{
    QString defDir = "";
    // try to read install path from registry
    QSettings registry("HKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432Node\\Valve\\Steam", QSettings::NativeFormat);
    defDir = QDir::toNativeSeparators(registry.value("InstallPath").toString());
    if (defDir.isEmpty()) {
        // fallback (32-bit system)
        QSettings registry2("HKEY_LOCAL_MACHINE\\SOFTWARE\\Valve\\Steam", QSettings::NativeFormat);
        defDir = QDir::toNativeSeparators(registry2.value("InstallPath").toString());
    }
    if (!defDir.isEmpty()) cbDirectory->addItem(defDir);
}

void SteamPathPage::browse()
{
    QString directory =
            QDir::toNativeSeparators(QFileDialog::getExistingDirectory(this, "", cbDirectory->currentText()));

    if (!directory.isEmpty()) {
        if (cbDirectory->findText(directory) == -1)
            cbDirectory->addItem(directory);
        cbDirectory->setCurrentIndex(cbDirectory->findText(directory));
    }
}

bool SteamPathPage::isComplete() const
{
    QString chk = QDir::toNativeSeparators(cbDirectory->currentText());
    if (chk.isEmpty())
        return false;
    return QDir(chk).exists();
}
