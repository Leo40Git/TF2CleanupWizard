#ifndef SELECTUSERPAGE_H
#define SELECTUSERPAGE_H

#include <QWizardPage>
#include <QListView>
#include <QStandardItemModel>
#include <QThread>

class QModelIndex;

class SelectUserPage : public QWizardPage
{
    Q_OBJECT
public:
    explicit SelectUserPage(QWidget *parent = nullptr);
    ~SelectUserPage() override;
    void initializePage() override;
    bool isComplete() const override;

private:
    void parseErrorAndQuit(const QString &path, const QString &error);
    void accountSelected(const QModelIndex &current, const QModelIndex &previous);
    QListView *lvAccounts;
    bool steamIDSet;
    QThread workerThread;

private slots:
    void resultsReady(QStandardItemModel *model, QModelIndex defaultIndex);

signals:
    void parseUsersVDF();

};

#endif // SELECTUSERPAGE_H
