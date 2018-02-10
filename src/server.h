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

#include "morgoth_export.h"
#include "serverconfiguration.h"
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QUrl>
#include <QtDBus/QDBusObjectPath>

namespace morgoth {

class ServerCoordinator;
class ServerStatus;

/**
 * \brief The Server class represents a single server installation.
 *
 * The \c Server instance contains all static information about the server.
 * A single server is identified by its \ref name. Its installation \ref path
 * contains the \c srcds_run script. To start or stop the server, execute
 * any command or access its status, use \ref ServerCoordinator.
 *
 * \sa ServerCoordinator and ServerManager.
 * \todo Handle remote servers.
 */
class MORGOTH_EXPORT Server : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.morgoth.Server")

    /**
     * Name of this server.
     */
    Q_PROPERTY(QString name READ name CONSTANT)

    /**
     * Path to this server's installation.
     */
    Q_PROPERTY(QUrl path READ path CONSTANT)

    /**
     * Specifies whether this server is valid.
     */
    Q_PROPERTY(bool valid READ isValid CONSTANT)

    /**
     * This server's coordinator instance.
     */
    Q_PROPERTY(ServerCoordinator* coordinator READ coordinator CONSTANT)

    /**
     * This server's coordinator D-Bus path.
     */
    Q_PROPERTY(QDBusObjectPath coordinatorPath READ coordinatorPath CONSTANT)

    /**
     * This server's configuration.
     */
    Q_PROPERTY(ServerConfiguration* configuration READ configuration CONSTANT)

    /**
     * This server's configuration D-Bus path.
     */
    Q_PROPERTY(QDBusObjectPath configurationPath READ configurationPath CONSTANT)

    /**
     * This server's status.
     */
    Q_PROPERTY(ServerStatus* status READ status CONSTANT)

    /**
     * This server's status object D-Bus path.
     */
    Q_PROPERTY(QDBusObjectPath statusPath READ statusPath CONSTANT)

public:
    /**
     * \brief Creates the new \c Server instance.
     * \param path Path to the directory where the server is installed.
     * \param name Name of the server.
     * \param parent Passed to QObject.
     * \warning Do _not_ use this constructor unless you know exactly what you
     *  are doing.
     * \sa ServerManager::add().
     */
    Server(const QUrl& path, const QString& name, QObject* parent = nullptr);

    /**
     * \brief Deletes this \c Server instance.
     */
    virtual ~Server();

    /**
     * \brief Returns path to the srcds_run script.
     * \note This function returns an empty string if \ref isValid() is \c false.
     */
    const QString& srcdsExec() const { return m_srcdsExec; }

    const QString& name() const { return m_name; }
    const QUrl& path() const { return m_path; }
    bool isValid() const { return m_valid; }
    ServerCoordinator* coordinator() { return m_coordinator; }
    const ServerCoordinator* coordinator() const { return m_coordinator; }
    const QDBusObjectPath& coordinatorPath() const { return m_coordinatorPath; }
    ServerConfiguration* configuration() { return m_configuration; }
    const ServerConfiguration* configuration() const { return m_configuration; }
    const QDBusObjectPath& configurationPath() const { return m_configurationPath; }
    ServerStatus* status() { return m_status; }
    const ServerStatus* status() const { return m_status; }
    const QDBusObjectPath& statusPath() const { return m_statusPath; }

private:
    void discover();
    void initializeDefaultConfiguration();

    QString m_name;
    QUrl m_path;
    bool m_valid;
    QString m_srcdsExec;
    ServerCoordinator* m_coordinator = nullptr;
    QDBusObjectPath m_coordinatorPath;
    ServerConfiguration* m_configuration = nullptr;
    QDBusObjectPath m_configurationPath;
    ServerStatus* m_status = nullptr;
    QDBusObjectPath m_statusPath;

};

} // namespace Morgoth

#endif // SERVER_H
