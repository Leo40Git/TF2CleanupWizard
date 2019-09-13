#ifndef SELECTUSERPAGE_H
#define SELECTUSERPAGE_H

#include <QWizardPage>
#include <QListView>

class SelectUserPage : public QWizardPage
{
    Q_OBJECT
public:
    explicit SelectUserPage(QWidget *parent = nullptr);
    void initializePage() override;
    bool isComplete() const override;
private:
    QListView *lvAccounts;
    bool gotAccount;
};

#endif // SELECTUSERPAGE_H
