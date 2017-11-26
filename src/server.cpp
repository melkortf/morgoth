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

Server::Server(const QString& path, const QString& name, QObject* parent) :
    QObject(parent),
    m_name(name),
    m_path(path),
    m_srcdsExec(path + "/srcds_run"),
    m_coordinator(new ServerCoordinator(this))
{
    discover();
    new dbus::ServerAdaptor(this);
}

Server::~Server() {}

void Server::discover()
{
    m_valid = QFile::exists(m_srcdsExec);
}

} // namespace Morgoth
