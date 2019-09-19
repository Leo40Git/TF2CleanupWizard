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

    void setPathSteam(const QString& pathSteam);
    void setPathTF2(const QString& pathTF2);
    void setSteamID(quint64 steamID);

protected:
    void showEvent(QShowEvent *) override;

signals:
    void doWork(const QString &pathSteam, const QString &pathTF2, quint64 steamID);

private slots:
    void onDone();

private:
    Ui::CleanupDialog *ui;
    QString pathSteam, pathTF2;
    quint64 steamID;
    QThread workerThread;
};

#endif // CLEANUPDIALOG_H
