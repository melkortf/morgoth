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

#include "servermanager.h"
#include "configuration.h"
#include "morgothdaemon.h"
#include "servermanageradaptor.h"
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

    new ServerManagerAdaptor(this);
    qApp->dbusConnection()->registerObject("/serverManager", this);
}

Server* ServerManager::find(const QString& name) const
{
    auto it = std::find_if(m_servers.begin(), m_servers.end(), [&name](auto s) {
        return s->name() == name;
    });
    return it == m_servers.end() ? nullptr : *it;
}

Server* ServerManager::add(const QUrl& path, const QString& name)
{
    if (find(name) != nullptr) {
        qWarning("Could not add server \"%s\": name already exists", qPrintable(name));
        return nullptr;
    }

    QUrl fixedPath(path);
    if (!QFile::exists(fixedPath.toString())) {
        fixedPath = QUrl::fromLocalFile(fixedPath.toString());
    }

    Server* s = new Server(path, name, this);

    QSqlRecord record = m_model.record();
    record.setGenerated("id", true);
    record.setValue("name", name);
    record.setValue("path", path);

    ServerLaunchArguments launchArguments;
    record.setValue("launchArguments", launchArguments.asSrcdsArguments());

    bool ret = m_model.insertRecord(-1, record);
    if (!ret) {
        QSqlError error = m_database.lastError();
        qWarning("Error adding \"%s\": %s", qPrintable(name), qPrintable(error.text()));
        delete s;
        return nullptr;
    }

    m_servers.append(s);
    emit serverAdded(s);

    return s;
}

QStringList ServerManager::serverNames() const
{
    QStringList result;
    std::for_each(m_servers.begin(), m_servers.end(), [&result](const Server* s) {
        result.append(s->name());
    });

    return result;
}

void ServerManager::initializeServers()
{
    m_database.exec("CREATE TABLE IF NOT EXISTS servers(id integer primary key autoincrement, path text, name text, launchArguments text)");

    m_model.setTable("servers");
    m_model.select();

    for (int i = 0; i < m_model.rowCount(); ++i) {
        QSqlRecord record = m_model.record(i);
        QUrl path = record.value("path").toUrl();
        QString name = record.value("name").toString();
        QString launchArguments = record.value("launchArguments").toString();

        Server* s = new Server(path, name, this);
        s->setLaunchArguments(ServerLaunchArguments::fromSrcdsArguments(launchArguments));
        m_servers.append(s);
    }

    std::for_each(m_servers.begin(), m_servers.end(), [](auto s) {
        qInfo("%s: %s", qPrintable(s->name()), (s->isValid() ? qPrintable(s->path().toString()) : "NOT FOUND"));
    });
}

} // namespace Morgoth
