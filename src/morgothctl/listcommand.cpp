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

#include "listcommand.h"
#include "morgothdaemon.h"
#include "serverinterface.h"
#include "servermanagerinterface.h"
#include <QtDBus>
#include <tuple>

int ListCommand::execute(QDBusConnection dbus, const QStringList& arguments, QTextStream& out)
{
    // todo list only selected servers
    Q_UNUSED(arguments);

    org::morgoth::ServerManager serverManager(morgoth::MorgothDaemon::dbusServiceName(), "/servers", dbus, qApp);
    Q_ASSERT(serverManager.isValid());

    QStringList servers = serverManager.servers();
    if (servers.isEmpty()) {
        out << "No servers installed." << endl;
        return 0;
    }

    using OutputType = std::tuple<QString, QString>;
    QList<OutputType> output;

    for (const QString& s: qAsConst(servers)) {
        QDBusObjectPath path = serverManager.serverPath(s);
        org::morgoth::Server server(morgoth::MorgothDaemon::dbusServiceName(), path.path(), dbus);
        output.append(std::make_tuple(server.name(), server.path()));
    }

    // sort servers using their names
    std::sort(output.begin(), output.end(), [](auto a, auto b) {
        return std::get<0>(a) < std::get<0>(b);
    });

    output.prepend(std::make_tuple("-- name --", "-- path --"));

    // format output
    int namePadding = [&output]() -> int {
        auto e = std::max_element(output.begin(), output.end(), [](auto a, auto b) {
            return std::get<0>(a).size() < std::get<0>(b).size();
        });
        return std::get<0>(*e).size();
    }();

    int pathPadding = [&output]() -> int {
        auto e = std::max_element(output.begin(), output.end(), [](auto a, auto b) {
            return std::get<1>(a).size() < std::get<1>(b).size();
        });
        return std::get<1>(*e).size();
    }();

    for (auto o: output) {
        out << qSetFieldWidth(namePadding) << center << std::get<0>(o)
            << qSetFieldWidth(3) << " "
            << qSetFieldWidth(pathPadding) << center << std::get<1>(o)
            << qSetFieldWidth(0) << endl;
    }

    return 0;
}
