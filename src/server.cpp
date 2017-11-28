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

#include "server.h"
#include "servercoordinator.h"
#include "dbus/serveradaptor.h"
#include <QtCore>

namespace morgoth {

Server::Server(const QUrl& path, const QString& name, QObject* parent) :
    QObject(parent),
    m_name(name),
    m_path(path),
    m_coordinator(new ServerCoordinator(this))
{
    discover();
    new dbus::ServerAdaptor(this);
}

Server::~Server() {}

void Server::setLaunchArguments(const ServerLaunchArguments& launchArguments)
{
    m_launchArguments = launchArguments;
}

void Server::discover()
{
    m_valid = false;

    if (!m_path.isLocalFile()) {
        qWarning("Cannot handle server %s: remote servers not supported", qPrintable(m_path.toString()));
        return;
    }

    m_srcdsExec = m_path.toLocalFile() + QDir::separator() + "srcds_run";

    if (!QFile::exists(m_srcdsExec)) {
        qWarning("%s does not exist", qPrintable(m_srcdsExec));
        return;
    }

    m_valid = true;
}

} // namespace Morgoth
