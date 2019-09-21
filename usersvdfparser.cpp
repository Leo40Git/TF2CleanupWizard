#include "usersvdfparser.h"

UsersVDFParser::UsersVDFParser(const QString &pathSteam, QObject *parent) : QObject(parent)
{
    this->pathSteam = pathSteam;
}

void UsersVDFParser::doWork()
{
    QStandardItemModel *model = new QStandardItemModel(parent());
    // parse $pathSteam/config/loginusers.vdf
    QFile userVDF(QString("%1/config/loginusers.vdf").arg(pathSteam));
    if (!userVDF.open(QFile::ReadOnly | QFile::Text)) {
        emit error(userVDF.fileName(), "Could not open file for reading");
        return;
    }
    QString line;
    line = userVDF.readLine().chopped(1);
    if (QString::compare("\"users\"", line) != 0) {
        emit error(userVDF.fileName(), "Missing \"users\" key");
        return;
    }
    line = userVDF.readLine().chopped(1);
    if (QString::compare("{", line) != 0) {
        emit error(userVDF.fileName(), "Missing opening bracket");
        return;
    }

    QStandardItem *item = nullptr;
    QModelIndex defaultIndex;
    while (QString::compare("}", (line = userVDF.readLine().chopped(1))) != 0) {
        if (userVDF.atEnd()) {
            userVDF.close();
            emit error(userVDF.fileName(), "Missing closing bracket");
            return;
        }
        if (!item) {
            // line is new account declaration
            quint64 id = static_cast<quint64>(line.mid(2).chopped(1).toULongLong());
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
                defaultIndex = model->indexFromItem(item);
        } else if (QString::compare("\t}", line) == 0) {
            // line is end of account declaration
            item->setText(QString("<b>%1</b><br><i>%2</i>").arg(item->data(Qt::UserRole + 2).toString()).arg(item->data(Qt::UserRole + 3).toString()));
            item->setIcon(QIcon(QString("%1/config/avatarcache/%2").arg(pathSteam).arg(QString::number(item->data(Qt::UserRole + 1).toULongLong()))));
            item = nullptr;
        }
    }
    userVDF.close();
    emit resultsReady(model, defaultIndex);
}
