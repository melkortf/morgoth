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

#ifndef SERVERMANAGER_H
#define SERVERMANAGER_H

#include "server.h"
#include <QtCore/QObject>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlTableModel>

namespace morgoth {

/**
 * \brief The ServerManager class instantiates and gives access to all
 * setup servers.
 */
class ServerManager : public QObject {
    Q_OBJECT

signals:
    /**
     * \brief Emitted when a new server has been added.
     * \param server The new \c Server instance.
     */
    void serverAdded(Server* server);

public:
    /**
     * \brief Creates a new \c ServerManager instance.
     * \param parent Passed to QObject.
     */
    explicit ServerManager(QObject* parent = nullptr);

    /**
     * \brief Finds a \c Server instance by its name.
     * \param name The name of the server.
     * \return The \c Server instance or \c nullptr if nothing was found.
     */
    Server* find(const QString& name) const;

    /**
     * \brief Adds a new server instance.
     *
     * The server is tested for its validity (i.e. the existence of srcds_run
     * script) and added to the database.
     *
     * \param path Path to the installed server.
     * \param name Name of the new server.
     * \return The \c Server instance of the newly created server.
     */
    Server* add(const QUrl& path, const QString& name);

    /**
     * \brief Gives direct access to all installed servers.
     */
    const QList<Server*>& servers() const { return m_servers; }

private:
    void initializeServers();

    QList<Server*> m_servers;
    QSqlDatabase m_database;
    QSqlTableModel m_model;

};

} // namespace Morgoth

#endif // SERVERMANAGER_H
