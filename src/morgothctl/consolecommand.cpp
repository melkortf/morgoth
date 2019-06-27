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

#include "consolecommand.h"
#include "morgothdaemon.h"
#include "servercoordinator.h"
#include "servercoordinatorinterface.h"
#include "servermanagerinterface.h"
#include "serverinterface.h"
#include <QtCore>

int ConsoleCommand::execute(QDBusConnection dbus, const QStringList& arguments, QTextStream& out)
{
    QCommandLineParser parser;
    parser.setOptionsAfterPositionalArgumentsMode(QCommandLineParser::ParseAsPositionalArguments);
    parser.setApplicationDescription(QStringLiteral("%1: attach to server's console.").arg(command()));
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

    QDBusObjectPath path = serverManager.serverPath(serverName);
    Q_ASSERT(!path.path().isEmpty());
    org::morgoth::Server server(morgoth::MorgothDaemon::dbusServiceName(), path.path(), dbus);
    Q_ASSERT(server.isValid());

    org::morgoth::ServerCoordinator coordinator(morgoth::MorgothDaemon::dbusServiceName(), server.coordinatorPath().path(), dbus);
    if (coordinator.state() != morgoth::ServerCoordinator::Running) {
        out << "Server " << serverName << " is not running." << endl;
        return 3;
    }

    return 0;
}
