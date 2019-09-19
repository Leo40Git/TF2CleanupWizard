#ifndef CLEANUPWIZARD_H
#define CLEANUPWIZARD_H

#include <QWizard>

class CleanupWizard : public QWizard
{
    Q_OBJECT
    Q_PROPERTY(quint64 m_steamID READ steamID WRITE setSteamID NOTIFY steamIDChanged)
public:
    explicit CleanupWizard(QWidget *parent = nullptr);
    void accept() override;
    quint64 steamID();
    void setSteamID(quint64 steamID);
public slots:
    void done(int result) override;
private:
    quint64 m_steamID;
signals:
    void steamIDChanged();
};

#endif // CLEANUPWIZARD_H
