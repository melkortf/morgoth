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

#include "removecommand.h"
#include "morgothdaemon.h"
#include "servercoordinatorinterface.h"
#include "serverinterface.h"
#include "servermanagerinterface.h"
#include <QtCore>

int RemoveCommand::execute(QDBusConnection dbus, const QStringList& arguments, QTextStream& out)
{
    Q_UNUSED(out);

    QCommandLineParser parser;
    parser.setOptionsAfterPositionalArgumentsMode(QCommandLineParser::ParseAsPositionalArguments);
    parser.setApplicationDescription(QStringLiteral("%1: remove game server.").arg(command()));
    parser.addHelpOption();
    parser.addPositionalArgument("name", "The server name.", QStringLiteral("%1 <name>...").arg(command()));
    parser.process(arguments);

    QStringList args = parser.positionalArguments();
    if (args.isEmpty())
        parser.showHelp(1);

    org::morgoth::ServerManager serverManager(morgoth::MorgothDaemon::dbusServiceName(), "/servers", dbus, qApp);
    Q_ASSERT(serverManager.isValid());

    QString name = args.at(0);
    if (!serverManager.servers().contains(name)) {
        out << "Server \"" << name << "\" does not exist." << endl;
        return 2;
    }

    org::morgoth::Server server(morgoth::MorgothDaemon::dbusServiceName(), serverManager.serverPath(name).value().path(), dbus);
    if (server.valid()) {
        org::morgoth::ServerCoordinator coordinator(morgoth::MorgothDaemon::dbusServiceName(), server.coordinatorPath().path(), dbus);
        if (coordinator.state() == morgoth::ServerCoordinator::State::Running) {
            out << "Server \"" << name << "\" is currently running." << endl;
            return 3;
        }
    }

    bool ret = serverManager.remove(name);
    return ret ? 0 : 4;
}
