#include "selectuserpage.h"

#include <QVBoxLayout>
#include <QFile>
#include <QDir>
#include <QMessageBox>
#include <QApplication>
#include <QIcon>

#include "htmldelegate.h"
#include "usersvdfparser.h"

SelectUserPage::SelectUserPage(QWidget *parent) : QWizardPage(parent)
{
    setTitle("Select Account");
    setSubTitle("Please select your Steam account.\nIf it doesn't appear here, make sure you are logged in!");

    lvAccounts = new QListView(this);
    lvAccounts->setEditTriggers(QListView::NoEditTriggers);
    lvAccounts->setIconSize(QSize(32, 32));
    lvAccounts->setEnabled(false);
    QStandardItemModel *model = new QStandardItemModel(this);
    model->appendRow(new QStandardItem("Loading user list, please wait..."));
    lvAccounts->setModel(model);

    btnRefresh = new QPushButton("Refresh", this);
    connect(btnRefresh, &QPushButton::clicked, this, &SelectUserPage::refreshUsers);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(lvAccounts);
    layout->addWidget(btnRefresh);
}

SelectUserPage::~SelectUserPage()
{
    workerThread.quit();
    workerThread.wait();
}

void SelectUserPage::initializePage()
{
    steamIDSet = false;
    refreshUsers();
}

bool SelectUserPage::isComplete() const
{
    return steamIDSet;
}

void SelectUserPage::parseErrorAndQuit(const QString &path, const QString &error)
{
    QMessageBox::critical(this, "Failed to parse loginusers.vdf", QString("Could not parse \"%1\":\n%2\nThe wizard will now close. No changes have been made to your TF2 installation.").arg(path).arg(error));
    QApplication::quit();
}

void SelectUserPage::accountSelected(const QModelIndex &current, const QModelIndex &previous)
{
    (void)previous;
    quint64 steamID = static_cast<quint64>(current.data(Qt::UserRole + 1).toULongLong());
    setProperty("steamID", steamID);
    steamIDSet = true;
    emit completeChanged();
}

void SelectUserPage::refreshUsers()
{
    steamIDSet = false;
    emit completeChanged();
    lvAccounts->setEnabled(false);
    QStandardItemModel *model = new QStandardItemModel(this);
    model->appendRow(new QStandardItem("Loading user list, please wait..."));
    lvAccounts->setModel(model);
    btnRefresh->setEnabled(false);
    UsersVDFParser *worker = new UsersVDFParser(field("pathSteam").toString());
    worker->moveToThread(&workerThread);
    connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(worker, &UsersVDFParser::resultsReady, this, &SelectUserPage::resultsReady);
    connect(worker, &UsersVDFParser::error, this, &SelectUserPage::parseErrorAndQuit);
    connect(this, &SelectUserPage::parseUsersVDF, worker, &UsersVDFParser::doWork);
    workerThread.start();
    emit parseUsersVDF();
}

void SelectUserPage::resultsReady(QStandardItemModel *model, QModelIndex defaultIndex)
{
    lvAccounts->setModel(model);
    connect(lvAccounts->selectionModel(), &QItemSelectionModel::currentChanged, this, &SelectUserPage::accountSelected);
    if (defaultIndex.isValid()) {
        lvAccounts->setCurrentIndex(defaultIndex);
        lvAccounts->scrollTo(defaultIndex);
    }
    lvAccounts->setItemDelegate(new HTMLDelegate);
    lvAccounts->setEnabled(true);
    btnRefresh->setEnabled(true);
}
