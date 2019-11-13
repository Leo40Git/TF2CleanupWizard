#ifndef CLEANUPDIALOG_H
#define CLEANUPDIALOG_H

#include <QDialog>
#include <QThread>

namespace Ui {
class CleanupDialog;
}

class CleanupDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CleanupDialog(QWidget *parent = nullptr);
    ~CleanupDialog() override;

    void setPathSteam(const QString &pathSteam);
    void setPathTF2(const QString &pathTF2);
    void setSteamID(quint64 steamID);
    void setBackupFolderName(const QString &value);

protected:
    void showEvent(QShowEvent *) override;

signals:
    void doWork(const QString &pathSteam, const QString &pathTF2, quint64 steamID, const QString &backupFolderName);

private slots:
    void onDone();

private:
    Ui::CleanupDialog *ui;
    QString pathSteam, pathTF2;
    quint64 steamID;
    QString backupFolderName;
    QThread workerThread;
};

#endif // CLEANUPDIALOG_H
