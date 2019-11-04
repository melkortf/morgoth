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

#ifndef SERVERSTARTER_H
#define SERVERSTARTER_H

#include "morgoth_export.h"
#include "gameserverinterface.h"
#include "server.h"
#include "servererror.h"
#include "tmuxsessionwrapper.h"
#include <QtCore/QObject>
#include <QtCore/QTimer>

namespace morgoth {

class MORGOTH_NO_EXPORT ServerStarter : public QObject {
    Q_OBJECT

signals:
    void failed(ServerError error);
    void starting();
    void started(org::morgoth::connector::GameServer* gameServer);

public:
    explicit ServerStarter(Server* server, TmuxSessionWrapper* tmuxSession);
    virtual ~ServerStarter();

    void start();

private slots:
    void timeoutStart();
    void handleGameServerRegistered(org::morgoth::connector::GameServer* gameServer);

private:
    Server* m_server;
    TmuxSessionWrapper* m_tmuxSession;
    QTimer m_startTimeoutTimer;

};

} // namespace Morgoth

#endif // SERVERSTARTER_H
