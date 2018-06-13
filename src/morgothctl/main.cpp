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

#include "config.h"
#include "addcommand.h"
#include "configcommand.h"
#include "listcommand.h"
#include "morgothdaemon.h"
#include "removecommand.h"
#include "startcommand.h"
#include "statuscommand.h"
#include "stopcommand.h"
#include "servermanagerinterface.h"
#include <QtCore>
#include <algorithm>
#include <cstdio>

static const QString morgothService = morgoth::MorgothDaemon::dbusServiceName();
static QDBusConnection dbus = QDBusConnection::sessionBus();
static QTextStream qstdout(stdout);

bool findServerManager()
{
    org::morgoth::ServerManager* serverManager;

    // try to connect to the service on the session bus first, then on the system one
    serverManager = new org::morgoth::ServerManager(morgothService, "/servers", dbus, qApp);
    if (!serverManager->isValid()) {
        delete serverManager;

        dbus = QDBusConnection::systemBus();
        serverManager = new org::morgoth::ServerManager(morgothService, "/servers", dbus, qApp);

        if (!serverManager->isValid())
            return false;
    }

    return true;
}

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    app.setApplicationName("morgothctl");
    app.setApplicationVersion(MORGOTH_VERSION);

    QList<morgoth::MorgothCtlPlugin*> commands({
        new ListCommand,
        new AddCommand,
        new RemoveCommand,
        new StartCommand,
        new StopCommand,
        new StatusCommand,
        new ConfigCommand
    });

    QCommandLineParser parser;
    parser.setOptionsAfterPositionalArgumentsMode(QCommandLineParser::ParseAsPositionalArguments);
    parser.setApplicationDescription("A morgoth control program.");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("command", "The command to execute.", "<command> [<args>]");
    parser.process(app.arguments());

    QStringList args = parser.positionalArguments();
    if (args.isEmpty())
        parser.showHelp(1);

    if (!findServerManager()) {
        qstdout << "Couldn't access morgoth daemon over its D-Bus interface; is morgothd running?" << endl;
        return 1;
    }

    QString command = args.first();

    auto it = std::find_if(commands.begin(), commands.end(), [&command](auto c) {
        return c->command() == command;
    });

    int ret = 0;
    if (it == commands.end()) {
        qstdout << "'" << command << "' is not a morgothctl command. See 'morgothctl --help'." << endl;
        ret = 1;
    } else {
        ret = (*it)->execute(dbus, args, qstdout);
    }

    return ret;
}
