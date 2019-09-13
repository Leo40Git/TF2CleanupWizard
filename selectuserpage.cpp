#include "selectuserpage.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QStandardItemModel>

#include "htmldelegate.h"

SelectUserPage::SelectUserPage(QWidget *parent) : QWizardPage(parent)
{
    setTitle("Select Account");

    QLabel *label = new QLabel("Please select your account:\n"
                               "(If your account does not appear in this list, "
                               "make sure you are logged into Steam with it!)", this);

    lvAccounts = new QListView(this);
    lvAccounts->setEditTriggers(QListView::NoEditTriggers);
    setField("accountID", 0ull); // accountID format is steamID64 (aka uint64)

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(label);
    layout->addWidget(lvAccounts);
}

void SelectUserPage::initializePage()
{
    QStandardItemModel *model = new QStandardItemModel(this);
    model->appendRow(new QStandardItem("<b>Leo40Gamer</b><br><i>kfir40gamer</i>"));
    lvAccounts->setModel(model);
    lvAccounts->setItemDelegate(new HTMLDelegate);
}

bool SelectUserPage::isComplete() const
{
    return gotAccount;
}
