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

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(lvAccounts);
}

SelectUserPage::~SelectUserPage()
{
    workerThread.quit();
    workerThread.wait();
}

void SelectUserPage::initializePage()
{
    steamIDSet = false;
    QStandardItemModel *model = new QStandardItemModel(this);
    model->appendRow(new QStandardItem("Loading user list, please wait..."));
    lvAccounts->setModel(model);

    UsersVDFParser *worker = new UsersVDFParser(field("pathSteam").toString());
    worker->moveToThread(&workerThread);
    connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(worker, &UsersVDFParser::resultsReady, this, &SelectUserPage::resultsReady);
    connect(worker, &UsersVDFParser::error, this, &SelectUserPage::parseErrorAndQuit);
    connect(this, &SelectUserPage::parseUsersVDF, worker, &UsersVDFParser::doWork);
    workerThread.start();
    emit parseUsersVDF();
}

bool SelectUserPage::isComplete() const
{
    return steamIDSet;
}

void SelectUserPage::parseErrorAndQuit(const QString &path, const QString &error)
{
    QMessageBox::critical(this, "Failed to parse loginusers.vdf", "Could not parse \"" + path + "\":\n" + error + "\nInstaller will now close. No changes have been done.");
    QApplication::quit();
}

void SelectUserPage::accountSelected(const QModelIndex &current, const QModelIndex &previous)
{
    (void)previous;
    quint64 steamID = static_cast<quint64>(current.data(Qt::UserRole + 1).toULongLong());
    wizard()->setProperty("steamID", steamID);
    steamIDSet = true;
    emit completeChanged();
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
}
