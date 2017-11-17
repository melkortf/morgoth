#include "servermanager.h"
#include <QtCore>
#include <QtSql>
#include <algorithm>
#include <iostream>

namespace morgoth {

ServerManager::ServerManager(QObject* parent) :
    QObject(parent)
{
    initializeServers();
}

Server* ServerManager::find(const QString& name) const
{
    auto it = std::find_if(m_servers.begin(), m_servers.end(), [&name](auto s) {
        return s->name() == name;
    });
    return *it;
}

void ServerManager::initializeServers()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("morgoth.sqlite");
    if (!db.open()) {
        qFatal("Could not connect to database; exiting...");
    }

    QSqlTableModel model;
    model.setTable("servers");
    model.select();

    for (int i = 0; i < model.rowCount(); ++i) {
        QSqlRecord record = model.record(i);
        QString path = record.value("path").toString();
        QString name = record.value("name").toString();

        Server* s = new Server(path, name, this);
        m_servers.push_back(s);
    }

    std::for_each(m_servers.begin(), m_servers.end(), [](auto s) {
        qInfo("%s: %s", qPrintable(s->name()), (s->isValid() ? qPrintable(s->path()) : "NOT FOUND"));
    });

    db.close();
}

} // namespace Morgoth
