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

#include "addcommand.h"
#include "morgothdaemon.h"
#include "servermanagerinterface.h"
#include <QtCore>

int AddCommand::execute(QDBusConnection dbus, const QStringList& arguments, QTextStream& out)
{
    Q_UNUSED(out);

    QCommandLineParser parser;
    parser.setOptionsAfterPositionalArgumentsMode(QCommandLineParser::ParseAsPositionalArguments);
    parser.setApplicationDescription(QStringLiteral("%1: add game server.").arg(command()));
    parser.addHelpOption();
    parser.addPositionalArgument("name", "The server name.", QStringLiteral("%1 <name>...").arg(command()));
    parser.addPositionalArgument("path", "The server installation path.", QStringLiteral("<path>"));
    parser.process(arguments);

    QStringList args = parser.positionalArguments();
    if (args.size() != 2)
        parser.showHelp(1);

    org::morgoth::ServerManager serverManager(morgoth::MorgothDaemon::dbusServiceName(), "/servers", dbus, qApp);
    Q_ASSERT(serverManager.isValid());

    QString name = args.at(0);
    QString path = args.at(1);

    bool ret = serverManager.add(path, name);
    return ret ? 0 : 2;
}
