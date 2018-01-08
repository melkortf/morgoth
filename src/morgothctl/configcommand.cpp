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

#include "configcommand.h"
#include "morgothdaemon.h"
#include "serverconfigurationinterface.h"
#include "servermanagerinterface.h"
#include "serverinterface.h"
#include <QtCore>

int ConfigCommand::execute(QDBusConnection dbus, const QStringList& arguments, QTextStream& out)
{
    QCommandLineParser parser;
    parser.setOptionsAfterPositionalArgumentsMode(QCommandLineParser::ParseAsPositionalArguments);
    parser.setApplicationDescription("config - set/get a server's configuration");
    parser.addHelpOption();
    parser.addPositionalArgument("name", "The server name");
    parser.addPositionalArgument("key", "The config key");
    parser.addPositionalArgument("value", "The config value");
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

    org::morgoth::Server server(morgoth::MorgothDaemon::dbusServiceName(), serverManager.serverPath(serverName).value().path(), dbus);
    org::morgoth::ServerConfiguration configuration(morgoth::MorgothDaemon::dbusServiceName(), server.configurationPath().path(), dbus);

    QStringList keys = configuration.keys();
    if (args.size() == 1) {
        // list the whole configuration
        int padding = std::max_element(keys.begin(), keys.end(), [](auto a, auto b) { return a.size() < b.size(); })->size();
        for (const QString& key: qAsConst(keys)) {
            out << qSetFieldWidth(padding) << right << key
                << qSetFieldWidth(0) << ": " << left
                << configuration.value(key) << endl;
        }
    } else if (args.size() == 2) {
        QString key = args.at(1);
        out << configuration.value(key) << endl;
    } else if (args.size() == 3) {
        QString key = args.at(1);
        QString value = args.at(2);
        configuration.setValue(key, value);
    } else {
        parser.showHelp(1);
    }

    return 0;
}
