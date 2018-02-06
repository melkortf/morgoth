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

#include "statuscommand.h"
#include "morgothdaemon.h"
#include "servercoordinatorinterface.h"
#include "serverinterface.h"
#include "servermanagerinterface.h"
#include <QtCore>

int StatusCommand::execute(QDBusConnection dbus, const QStringList& arguments, QTextStream& out)
{
    QCommandLineParser parser;
    parser.setOptionsAfterPositionalArgumentsMode(QCommandLineParser::ParseAsPositionalArguments);
    parser.setApplicationDescription(QStringLiteral("%1: show server status.").arg(command()));
    parser.addHelpOption();
    parser.addPositionalArgument("name", "The server name.", QStringLiteral("%1 <name>...").arg(command()));
    parser.process(arguments);

    QStringList args = parser.positionalArguments();
    if (args.isEmpty())
        parser.showHelp(1);

    org::morgoth::ServerManager serverManager(morgoth::MorgothDaemon::dbusServiceName(), "/servers", dbus, qApp);
    Q_ASSERT(serverManager.isValid());

    QString serverName = args.first();
    if (!serverManager.servers().contains(serverName)) {
        out << "Server \"" << serverName << "\" does not exist." << endl;
        return 2;
    }

    QString status;
    org::morgoth::Server server(morgoth::MorgothDaemon::dbusServiceName(), serverManager.serverPath(serverName).value().path(), dbus);
    if (server.valid()) {
        org::morgoth::ServerCoordinator coordinator(morgoth::MorgothDaemon::dbusServiceName(), server.coordinatorPath().path(), dbus);
        switch (coordinator.state()) {
            case morgoth::ServerCoordinator::State::Offline:
                status = "offline";
                break;

            case morgoth::ServerCoordinator::State::Starting:
                status = "starting";
                break;

            case morgoth::ServerCoordinator::State::Running:
                status = "running";
                break;

            case morgoth::ServerCoordinator::State::ShuttingDown:
                status = "shutting down";
                break;

            case morgoth::ServerCoordinator::State::Crashed:
                status = "crashed";
                break;
        }
    } else {
        status = "invalid";
    }

    out << server.name() << ": " << status << endl;
    return 0;
}
