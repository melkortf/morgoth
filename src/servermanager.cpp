#include "servermanager.h"
#include "configuration.h"
#include "dbus/servermanageradaptor.h"
#include <QtCore>
#include <QtSql>
#include <algorithm>
#include <iostream>

namespace morgoth {

ServerManager::ServerManager(QObject* parent) :
    QObject(parent),
    m_database(QSqlDatabase::addDatabase("QSQLITE"))
{
    Configuration* config = qApp->property("configuration").value<Configuration*>();
    QString databaseFile = config->value("database", "morgoth.sqlite").toString();
    qDebug("Using database file: %s", qPrintable(databaseFile));
    m_database.setDatabaseName(databaseFile);
    if (!m_database.open()) {
        qFatal("Could not connect to database; exiting...");
    }

    initializeServers();

    new dbus::ServerManagerAdaptor(this);
}

Server* ServerManager::find(const QString& name) const
{
    auto it = std::find_if(m_servers.begin(), m_servers.end(), [&name](auto s) {
        return s->name() == name;
    });
    return it == m_servers.end() ? nullptr : *it;
}

bool ServerManager::add(const QString& path, const QString& name)
{
    if (find(name) != nullptr) {
        qWarning("Could not add server \"%s\": name already exists", qPrintable(name));
        return false;
    }

    Server* s = new Server(path, name, this);

    QSqlRecord record = m_model.record();
    record.setGenerated("id", true);
    record.setValue("name", name);
    record.setValue("path", path);
    bool ret = m_model.insertRecord(-1, record);
    if (!ret) {
        QSqlError error = m_database.lastError();
        qWarning("Error adding \"%s\": %s", qPrintable(name), qPrintable(error.text()));
        return false;
    }

    m_servers.append(s);
    emit serverAdded(s);

    return true;
}

void ServerManager::initializeServers()
{
    m_database.exec("CREATE TABLE IF NOT EXISTS servers(id integer primary key autoincrement, path text, name text)");

    m_model.setTable("servers");
    m_model.select();

    for (int i = 0; i < m_model.rowCount(); ++i) {
        QSqlRecord record = m_model.record(i);
        QString path = record.value("path").toString();
        QString name = record.value("name").toString();

        Server* s = new Server(path, name, this);
        m_servers.append(s);
    }

    std::for_each(m_servers.begin(), m_servers.end(), [](auto s) {
        qInfo("%s: %s", qPrintable(s->name()), (s->isValid() ? qPrintable(s->path()) : "NOT FOUND"));
    });
}

} // namespace Morgoth
