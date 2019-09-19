#include "selectuserpage.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QStandardItemModel>
#include <QFile>
#include <QDir>
#include <QMessageBox>
#include <QApplication>
#include <QIcon>

#include "htmldelegate.h"

SelectUserPage::SelectUserPage(QWidget *parent) : QWizardPage(parent)
{
    setTitle("Select Account");

    QLabel *label = new QLabel("Please select your account:\n"
                               "(If your account does not appear in this list, "
                               "make sure you are logged into Steam with it!)", this);

    lvAccounts = new QListView(this);
    lvAccounts->setEditTriggers(QListView::NoEditTriggers);
    lvAccounts->setIconSize(QSize(32, 32));

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(label);
    layout->addWidget(lvAccounts);
}

void SelectUserPage::initializePage()
{
    steamIDSet = false;
    QStandardItemModel *model = new QStandardItemModel(this);
    // parse $pathSteam/config/loginusers.vdf
    QFile userVDF(field("pathSteam").toString() + "/config/loginusers.vdf");
    if (!userVDF.open(QFile::ReadOnly | QFile::Text))
        parseErrorAndQuit(userVDF.fileName(), "Could not open file for reading");
    QString line;
    line = userVDF.readLine().chopped(1);
    if (QString::compare("\"users\"", line) != 0) {
        parseErrorAndQuit(userVDF.fileName(), "Missing \"users\" key");
        return;
    }
    line = userVDF.readLine().chopped(1);
    if (QString::compare("{", line) != 0) {
        parseErrorAndQuit(userVDF.fileName(), "Missing opening bracket");
        return;
    }
    QStandardItem *item = nullptr;
    QModelIndex mostRecentIndex;
    while (QString::compare("}", (line = userVDF.readLine().chopped(1))) != 0) {
        if (userVDF.atEnd()) {
            userVDF.close();
            parseErrorAndQuit(userVDF.fileName(), "Missing closing bracket");
            return;
        }
        if (!item) {
            // line is new account declaration
            quint64 id = static_cast<quint64>(line.mid(2).chopped(1).toULongLong()); // strips \t" at start, " at end
            item = new QStandardItem();
            item->setData(id, Qt::UserRole + 1);
            model->appendRow(item);
        } else if (line.startsWith("\t\t\"PersonaName\"\t\t")) {
            // line is persona name
            item->setData(line.mid(18).chopped(1), Qt::UserRole + 2);
        } else if (line.startsWith("\t\t\"AccountName\"\t\t")) {
            // line is account name
            item->setData(line.mid(18).chopped(1), Qt::UserRole + 3);
        } else if (line.startsWith("\t\t\"mostrecent\"\t\t")) {
            // line is most recent
            if (QString::compare("1", line.mid(16).chopped(1)))
                mostRecentIndex = model->indexFromItem(item);
        } else if (QString::compare("\t}", line) == 0) {
            // line is end of account declaration
            item->setText(QString("<b>%1</b><br><i>%2</i>").arg(item->data(Qt::UserRole + 2).toString()).arg(item->data(Qt::UserRole + 3).toString()));
            item->setIcon(QIcon(field("pathSteam").toString() + "/config/avatarcache/" + QString::number(item->data(Qt::UserRole + 1).toULongLong())));
            item = nullptr;
        }
    }
    userVDF.close();
    lvAccounts->setModel(model);
    connect(lvAccounts->selectionModel(), &QItemSelectionModel::currentChanged, this, &SelectUserPage::accountSelected);
    if (mostRecentIndex.isValid()) {
        lvAccounts->setCurrentIndex(mostRecentIndex);
        lvAccounts->scrollTo(mostRecentIndex);
    }
    lvAccounts->setItemDelegate(new HTMLDelegate);
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
