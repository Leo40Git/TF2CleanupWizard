#include "tf2pathpage.h"

#include <QLabel>
#include <QPushButton>
#include <QDir>
#include <QFileDialog>
#include <QGridLayout>
#include <QSettings>

TF2PathPage::TF2PathPage(QWidget *parent) : QWizardPage(parent)
{
    setTitle("Select TF2 Path");

    QLabel *label = new QLabel("Please select your TF2 installation directory:", this);

    cbDirectory = new QComboBox(this);
    cbDirectory->setEditable(true);
    cbDirectory->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    registerField("pathTF2*", cbDirectory, "currentText", SIGNAL(currentTextChanged(QString)));

    QPushButton *button = new QPushButton("&Browse...", this);
    connect(button, &QPushButton::clicked, this, &TF2PathPage::browse);

    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget(label, 0, 0);
    layout->addWidget(cbDirectory, 1, 0);
    layout->addWidget(button, 1, 1);

    setLayout(layout);
}

void TF2PathPage::initializePage()
{
    QString defDir = QString("%1/steamapps/common/Team Fortress 2").arg(field("pathSteam").toString());
    cbDirectory->addItem(defDir);
}

void TF2PathPage::browse()
{
    QString directory =
            QFileDialog::getExistingDirectory(this, "", cbDirectory->currentText());

    if (!directory.isEmpty()) {
        if (cbDirectory->findText(directory) == -1)
            cbDirectory->addItem(directory);
        cbDirectory->setCurrentIndex(cbDirectory->findText(directory));
    }
}

bool TF2PathPage::isComplete() const
{
    QString chk = cbDirectory->currentText();
    if (chk.isEmpty()) {
        return false;
    }
    return QDir(chk).exists();
}
