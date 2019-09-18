#include "cleanupdialog.h"
#include "ui_cleanupdialog.h"

#include <QDir>
#include <QFile>
#include <QPushButton>

CleanupDialog::CleanupDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CleanupDialog)
{
    setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);

    ui->setupUi(this);

    connect(ui->btnFinish, &QPushButton::clicked, this, &QDialog::accept);
}

CleanupDialog::~CleanupDialog()
{
    delete ui;
}

void CleanupDialog::showEvent(QShowEvent *)
{
    QString path_tf = QDir::toNativeSeparators(pathTF2 + "/tf");
    QDir dir_tf(path_tf);
    ui->pteLog->appendPlainText(QString("renaming \"%1\" to \"%1_backup\"").arg(QDir::toNativeSeparators(path_tf + "/custom")));
    dir_tf.rename("custom", "custom_backup");
    ui->pteLog->appendPlainText(QString("creating directory \"%1\"").arg(QDir::toNativeSeparators(path_tf + "/cfg_backup")));
    dir_tf.mkdir("cfg_backup");
}

void CleanupDialog::setPathSteam(const QString &pathSteam)
{
    this->pathSteam = pathSteam;
}

void CleanupDialog::setPathTF2(const QString &pathTF2)
{
    this->pathTF2 = pathTF2;
}

void CleanupDialog::setSteamID(quint64 steamID)
{
    this->steamID = steamID;
}
