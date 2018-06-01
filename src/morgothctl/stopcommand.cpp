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

#include "stopcommand.h"
#include "morgothdaemon.h"
#include "servercoordinatorinterface.h"
#include "serverinterface.h"
#include "servermanagerinterface.h"
#include <QtCore>

int StopCommand::execute(QDBusConnection dbus, const QStringList& arguments, QTextStream& out)
{
    QCommandLineParser parser;
    parser.setOptionsAfterPositionalArgumentsMode(QCommandLineParser::ParseAsPositionalArguments);
    parser.setApplicationDescription(QStringLiteral("%1: stop game server(s).").arg(command()));
    parser.addHelpOption();
    parser.addPositionalArgument("name", "The server name.", QStringLiteral("%1 <name>...").arg(command()));
    parser.process(arguments);

    QStringList args = parser.positionalArguments();
    if (args.isEmpty())
        parser.showHelp(1);

    org::morgoth::ServerManager serverManager(morgoth::MorgothDaemon::dbusServiceName(), "/servers", dbus, qApp);
    Q_ASSERT(serverManager.isValid());

    QStringList servers = serverManager.servers();

    // validate all servers passed by the user
    for (const QString& serverName: qAsConst(args)) {
        if (!servers.contains(serverName)) {
            out << "Server \"" << serverName << "\" does not exist." << endl;
            return 2;
        }

        QDBusObjectPath path = serverManager.serverPath(serverName);
        org::morgoth::Server server(morgoth::MorgothDaemon::dbusServiceName(), path.path(), dbus);
        if (!server.valid()) {
            out << "Server \"" << serverName << "\" is invalid." << endl;
            return 3;
        }
    }

    // stop all the servers
    for (const QString& serverName: qAsConst(args)) {
        QDBusObjectPath path = serverManager.serverPath(serverName);
        Q_ASSERT(!path.path().isEmpty());
        org::morgoth::Server server(morgoth::MorgothDaemon::dbusServiceName(), path.path(), dbus);
        Q_ASSERT(server.isValid());

        org::morgoth::ServerCoordinator coordinator(morgoth::MorgothDaemon::dbusServiceName(), server.coordinatorPath().path(), dbus);
        if (coordinator.state() == morgoth::ServerCoordinator::State::Offline
                || coordinator.state() == morgoth::ServerCoordinator::State::Crashed) {
            out << "Server " << serverName << " is offline." << endl;
            return 0;
        }

        int ret = 0;
        QEventLoop loop;
        QObject::connect(&coordinator, &org::morgoth::ServerCoordinator::stateChanged, [&ret, &loop, &out, &serverName](auto state) {
            switch (state) {
                case morgoth::ServerCoordinator::Offline:
                    out << "stopped." << endl;
                    ret = 0;
                    loop.quit();
                    break;

                case morgoth::ServerCoordinator::ShuttingDown:
                    out << "Shutting down \"" << serverName << "\"... " << flush;
                    break;

                case morgoth::ServerCoordinator::Crashed:
                    out << "FAILED" << endl;
                    ret = 5;
                    loop.quit();
                    break;

                default:
                    break;
            }
        });

        coordinator.stop();
        loop.exec();
    }

    return 0;
}
