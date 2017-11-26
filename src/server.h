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

#ifndef SERVER_H
#define SERVER_H

#include <QtCore/QObject>
#include <QtCore/QString>

namespace morgoth {

class ServerCoordinator;

/**
 * \brief The Server class represents a single server installation.
 *
 * The \c Server instance contains all static information about the server.
 * A single server is identified by its \ref name. Its installation \ref path
 * contains the \c srcds_run script. To start or stop the server, execute
 * any command or access its status, use \ref ServerCoordinator.
 *
 * \sa ServerCoordinator and ServerManager.
 */
class Server : public QObject {
    Q_OBJECT

    /**
     * Name of this server.
     */
    Q_PROPERTY(QString name READ name CONSTANT)

    /**
     * Path to this server's installation.
     */
    Q_PROPERTY(QString path READ path CONSTANT)

    /**
     * This server's coordinator instance.
     */
    Q_PROPERTY(ServerCoordinator* coordinator READ coordinator CONSTANT)

public:
    /**
     * \brief Creates the new \c Server instance.
     * \param path Path to the server directory.
     * \param name Name of the server.
     * \param parent Passed to QObject.
     * \warning Do _not_ use this constructor unless you know exactly what you
     *  are doing.
     * \sa ServerManager::add().
     */
    Server(const QString& path, const QString& name, QObject* parent = nullptr);

    /**
     * \brief Deletes this \c Server instance.
     */
    virtual ~Server();

    /**
     * \brief Specifies whether this server is valid and therefore
     *  \ref coordinator is functional.
     */
    bool isValid() const { return m_valid; }

    /**
     * \brief Returns path to the srcds_run script.
     * \note This function returns an empty string if \ref isValid() is \c false.
     */
    const QString& srcdsExec() const { return m_srcdsExec; }

    const QString& name() const { return m_name; }
    const QString& path() const { return m_path; }
    ServerCoordinator* coordinator() { return m_coordinator; }
    const  ServerCoordinator* coordinator() const { return m_coordinator; }

private:
    void discover();

    QString m_name;
    QString m_path;
    bool m_valid;
    QString m_srcdsExec;
    ServerCoordinator* m_coordinator;

};

} // namespace Morgoth

#endif // SERVER_H
