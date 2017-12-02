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
    }
}

void Persistor::restoreServers()
{
    m_database.exec("CREATE TABLE IF NOT EXISTS servers(id integer primary key autoincrement, path text, name text)");

    QSqlQuery q;
    q.exec("SELECT path, name FROM servers");

    while (q.next()) {
        QUrl path = q.value(0).toUrl();
        QString name = q.value(1).toString();

        m_serverManager->add(path, name);
    }
}

void Persistor::storeServer(Server* server)
{
    QSqlQuery q;
    q.exec(QStringLiteral("SELECT * FROM servers WHERE NAME=%1").arg(server->name()));
    if (q.size() > 0) {
        qWarning("%s: server already exists in the database", qPrintable(server->name()));
        return;
    }

    q.prepare("INSERT INTO servers (path, name) VALUES (:path, :name)");
    q.bindValue(":path", server->path());
    q.bindValue(":name", server->name());
    if (!q.exec()) {
        qCritical("Error adding server: %s", qPrintable(q.lastError().text()));
    }
}

} // namespace morgoth
