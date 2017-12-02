// This file is part of morgoth.

// morgoth is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "persistor.h"
#include "morgothdaemon.h"
#include <QtCore>
#include <QtSql>

namespace morgoth {

Persistor::Persistor(ServerManager* serverManager) :
    QObject(serverManager),
    m_serverManager(serverManager),
    m_database(QSqlDatabase::addDatabase("QSQLITE"))
{
    initializeDatabase();
    restoreServers();

    connect(serverManager, &ServerManager::serverAdded, this, &Persistor::storeServer);
}

void Persistor::initializeDatabase()
{
    QString databaseFile = morgothd->config().value("database", "morgoth.sqlite").toString();
    qDebug("Using database file: %s", qPrintable(databaseFile));
    m_database.setDatabaseName(databaseFile);
    if (!m_database.open()) {
        qCritical() << m_database.lastError();
        qCritical("Could not connect to database");
    } else {
        Q_ASSERT(m_database.driver()->hasFeature(QSqlDriver::LastInsertId));
        Q_ASSERT(m_database.driver()->hasFeature(QSqlDriver::Transactions));

        m_database.exec("CREATE TABLE IF NOT EXISTS servers(id integer primary key autoincrement, path text, name text)");
        m_database.exec("CREATE TABLE IF NOT EXISTS server_configuration_entries("
                        "id integer primary key autoincrement, "
                        "serverid integer, "
                        "key string, "
                        "value string)");
    }
}

void Persistor::restoreServers() {
    QSqlQuery q;
    q.exec("SELECT id, path, name FROM servers");

    QSqlQuery q2;
    q2.prepare("SELECT key, value FROM server_configuration_entries WHERE serverid=:serverid");

    while (q.next()) {
        QVariant id = q.value(0);
        QUrl path = q.value(1).toUrl();
        QString name = q.value(2).toString();

        Server* server = m_serverManager->add(path, name);
        ServerConfiguration* configuration = server->configuration();
        q2.bindValue(":serverid", id);
        if (q2.exec()) {
            while (q2.next()) {
                QString key = q2.value(0).toString();
                QString value = q2.value(1).toString();
                configuration->setValue(key, value);
            }
        }

        connect(configuration, &ServerConfiguration::valueChanged, this, &Persistor::storeConfigurationEntry);
    }
}

void Persistor::storeServer(Server* server)
{
    QSqlQuery q;
    q.exec(QStringLiteral("SELECT * FROM servers WHERE name='%1'").arg(server->name()));
    if (q.size() > 0) {
        qWarning("%s: server already exists in the database", qPrintable(server->name()));
        return;
    }

    m_database.transaction();

    q.prepare("INSERT INTO servers (path, name) VALUES (:path, :name)");
    q.bindValue(":path", server->path());
    q.bindValue(":name", server->name());
    if (!q.exec()) {
        qCritical("Error adding server: %s", qPrintable(q.lastError().text()));
        m_database.rollback();
        return;
    }

    if (!q.lastInsertId().isValid()) {
        qCritical("Error adding server: could not retrieve lastInsertId");
        m_database.rollback();
        return;
    }

    QVariant id = q.lastInsertId();
    q.prepare("INSERT INTO server_configuration_entries(serverid, key, value) VALUES (:serverid, :key, :value)");

    QStringList keys = server->configuration()->keys();
    for (const QString& key: qAsConst(keys)) {
        QVariant value = server->configuration()->value(key);
        q.bindValue(":serverid", id);
        q.bindValue(":key", key);
        q.bindValue(":value", value);
        if (!q.exec()) {
            qCritical("Error adding server configration: %s", qPrintable(q.lastError().text()));
            m_database.rollback();
            return;
        }
    }

    m_database.commit();

    connect(server->configuration(), &ServerConfiguration::valueChanged, this, &Persistor::storeConfigurationEntry);
}

void Persistor::storeConfigurationEntry(const QString& key, const QString& value)
{
    ServerConfiguration* configuration = qobject_cast<ServerConfiguration*>(sender());
    Q_ASSERT(configuration);

    QSqlQuery q;
    bool res = q.exec(QStringLiteral("SELECT id FROM server_configuration_entries WHERE serverid=("
                                     "SELECT id FROM servers WHERE name='%1'"
                                     ") AND key='%2'").arg(configuration->server()->name(), key));
    if (!res) {
        qCritical() << q.lastQuery();
        qCritical() << q.lastError();
        return;
    }

    if (q.size() > 0) {
        QVariant id = q.value(0);
        res = q.exec(QStringLiteral("UPDATE server_configuration_entries SET value='%1' WHERE id=%2").arg(value, id.toString()));
    } else {
        res = q.exec(QStringLiteral("INSERT INTO server_configuration_entries(serverid, key, value) VALUES (("
                                    "SELECT id FROM servers WHERE name='%1'"
                                    "), '%2', '%3')").arg(configuration->server()->name(), key, value));
    }

    if (!res) {
        qCritical() << q.lastQuery();
        qCritical() << q.lastError();
    }
}

} // namespace morgoth
