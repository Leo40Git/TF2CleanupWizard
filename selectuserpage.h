#ifndef SELECTUSERPAGE_H
#define SELECTUSERPAGE_H

#include <QWizardPage>
#include <QListView>

class QModelIndex;

class SelectUserPage : public QWizardPage
{
    Q_OBJECT
public:
    explicit SelectUserPage(QWidget *parent = nullptr);
    void initializePage() override;
    bool isComplete() const override;
private:
    void parseErrorAndQuit(const QString &path, const QString &error);
    void accountSelected(const QModelIndex &current, const QModelIndex &previous);
    QListView *lvAccounts;
    quint64 accountID;
    bool accountIDSet;
signals:
    void accountIDChanged();
};

#endif // SELECTUSERPAGE_H
