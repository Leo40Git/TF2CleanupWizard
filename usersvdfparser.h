#ifndef USERSVDFPARSER_H
#define USERSVDFPARSER_H

#include <QObject>
#include <QList>
#include <QStandardItemModel>
#include <QModelIndex>

class UsersVDFParser : public QObject
{
    Q_OBJECT
public:
    explicit UsersVDFParser(const QString &pathSteam, QObject *parent = nullptr);
signals:
    void error(const QString &path, const QString &error);
    void resultsReady(QStandardItemModel *model, QModelIndex defaultIndex);
public slots:
    void doWork();
private:
    QString pathSteam;
};

#endif // USERSVDFPARSER_H
