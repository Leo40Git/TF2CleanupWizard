#include "cleanupdialog.h"
#include "ui_cleanupdialog.h"

#include <QDir>
#include <QFile>
#include <QPushButton>

#include "cleanupworker.h"

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
    workerThread.quit();
    workerThread.wait();
}

void CleanupDialog::showEvent(QShowEvent *e)
{
    CleanupWorker *worker = new CleanupWorker;
    worker->moveToThread(&workerThread);
    connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(worker, &CleanupWorker::done, this, &CleanupDialog::onDone);
    connect(worker, &CleanupWorker::log, ui->pteLog, &QPlainTextEdit::appendPlainText);
    connect(worker, &CleanupWorker::setProgressLabel, ui->lblProgress, &QLabel::setText);
    connect(worker, &CleanupWorker::setProgressValue, ui->progressBar, &QProgressBar::setValue);
    connect(worker, &CleanupWorker::setProgressMaximum, ui->progressBar, &QProgressBar::setMaximum);
    connect(worker, &CleanupWorker::setProgressFormat, ui->progressBar, &QProgressBar::setFormat);
    connect(this, &CleanupDialog::doWork, worker, &CleanupWorker::doWork);
    QDialog::showEvent(e);
    workerThread.start();
    emit doWork(pathSteam, pathTF2, steamID, backupFolderName);
}

void CleanupDialog::onDone()
{
    ui->btnFinish->setEnabled(true);
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

void CleanupDialog::setBackupFolderName(const QString &value)
{
    backupFolderName = value;
}
