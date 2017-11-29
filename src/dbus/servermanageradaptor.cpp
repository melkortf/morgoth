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

#include "servermanageradaptor.h"
#include "serveradaptor.h"
#include "../morgothdaemon.h"
#include "../servercoordinator.h"
#include <QtCore>
#include <QtDBus>

namespace morgoth { namespace dbus {

ServerManagerAdaptor::ServerManagerAdaptor(ServerManager* serverManager) :
    QDBusAbstractAdaptor(serverManager),
    m_serverManager(serverManager)
{
    qApp->dbusConnection()->registerObject("/servers", serverManager);

    connect(m_serverManager, &ServerManager::serverAdded, this, &ServerManagerAdaptor::handleNewServer);
}

ServerManagerAdaptor::~ServerManagerAdaptor() {}

QStringList ServerManagerAdaptor::servers() const
{
    QStringList servers;
    for (Server* s: m_serverManager->servers())
        servers << s->name();

    return servers;
}

bool ServerManagerAdaptor::add(QString path, QString name)
{
    return m_serverManager->add(QUrl::fromLocalFile(path), name) != nullptr;
}

void ServerManagerAdaptor::handleNewServer(Server* server)
{
    emit serverAdded(server->name());
}

}} // namespace morgoth::dbus
