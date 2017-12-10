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

#include "updatenotifier.h"
#include "morgothdaemon.h"
#include "updatenotificationevent.h"
#include "servercoordinator.h"
#include <QtCore>

namespace morgoth {

UpdateNotifier::UpdateNotifier(QObject *parent) :
    QObject(parent)
{
    ServerManager* sm = morgothd->property("servers").value<ServerManager*>();
    if (sm) {
        initialize(sm);
        qDebug() << "UpdateNotifier: loaded";
    } else {
        qWarning() << "UpdateNotifier: ** FAILED **";
    }
}

UpdateNotifier::~UpdateNotifier()
{
    qDebug() << "UpdateNotifier: unloaded";
}

void UpdateNotifier::initialize(ServerManager* serverManager)
{
    const QList<Server*>& servers = serverManager->servers();
    for (Server* s: qAsConst(servers)) {
        handleServer(s);
    }

    connect(serverManager, &ServerManager::serverAdded, this, &UpdateNotifier::handleServer);
}

void UpdateNotifier::handleServer(Server* server)
{
    UpdateNotificationEvent* update = new UpdateNotificationEvent;
    connect(update, &EventHandler::activated, [server]() { qInfo("%s: update available", qPrintable(server->name())); });
    server->coordinator()->installEventHandler(update);
}

} // namespace morgoth
