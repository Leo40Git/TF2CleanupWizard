#ifndef CLEANUPWORKER_H
#define CLEANUPWORKER_H

#include <QObject>
#include <QTimer>
#include <QDeadlineTimer>

class CleanupWorker : public QObject
{
    Q_OBJECT
signals:
    void log(const QString &msg);
    void setProgressLabel(const QString &text);
    void setProgressMaximum(int maximum);
    void setProgressValue(int value);
    void setProgressFormat(const QString &fmt);
    void done();
public slots:
    void doWork(const QString &pathSteam, const QString &pathTF2, quint64 steamID, const QString &backupFolderName);
private slots:
    void timerFire();
private:
    void doWork2();
    void deleteFilesAndValidate();
    void blankRemoteData();
    void launchWithCleanOptions();
    QString pathSteam;
    QString pathTF2;
    quint64 steamID;
    QString backupFolderName;
    QTimer *timer;
    QDeadlineTimer *deadline;
};

extern struct HWND__* hValidWnd;

#endif // CLEANUPWORKER_H
