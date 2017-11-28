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

#ifndef SERVERLAUNCHARGUMENTS_H
#define SERVERLAUNCHARGUMENTS_H

#include <QtCore/QMetaType>
#include <QtCore/QSharedData>
#include <QtCore/QString>

namespace morgoth {

class ServerLaunchArgumentsData;

/**
 * \brief The ServerLaunchArguments class describes arguments that are passeed
 *  to the server launch command.
 */
class ServerLaunchArguments {
public:
    /**
     * \brief Uses default arguments.
     *
     * Default arguments are:
     * * port = 27015,
     * * secured = \c true,
     * * initialMap is empty.
     */
    ServerLaunchArguments();

    /**
     * \brief Makes a copy of \c other.
     */
    ServerLaunchArguments(const ServerLaunchArguments& other);

    /**
     * \brief Destroys this \c ServerLaunchArguments instance.
     */
    virtual ~ServerLaunchArguments();

    /**
     * \brief Assigns \c other to \c this instance.
     */
    ServerLaunchArguments& operator=(const ServerLaunchArguments& other);

    /**
     * \brief Port on which the server will run.
     */
    unsigned port() const;

    /**
     * \brief Sets the port on which the server will run.
     */
    void setPort(unsigned port);

    /**
     * \brief Specifies whether this server is VAC-secured or not.
     */
    bool isSecured() const;

    /**
     * \brief Sets the VAC (Valve Anti-Cheat).
     */
    void setSecured(bool secured);

    /**
     * \brief The initial map on the server.
     */
    QString initialMap() const;

    /**
     * \brief Sets the initial map of the server.
     */
    void setInitialMap(const QString& initialMap);

    /**
     * \brief Combines arguments into one string.
     *
     * Combines all the arguments into one string, so that they can be passed
     * directly to the srcds_run script.
     */
    QString asSrcdsArguments() const;

    /**
     * \brief Constructs ServerLaunchArguments from the provided string.
     */
    static ServerLaunchArguments fromSrcdsArguments(const QString& srcdsArguments);

private:
    QSharedDataPointer<ServerLaunchArgumentsData> d;

};

} // namespace morgoth

QDebug operator<<(QDebug dbg, const morgoth::ServerLaunchArguments launchArguments);

Q_DECLARE_METATYPE(morgoth::ServerLaunchArguments)
Q_DECLARE_TYPEINFO(morgoth::ServerLaunchArguments, Q_MOVABLE_TYPE);

#endif // SERVERLAUNCHARGUMENTS_H
