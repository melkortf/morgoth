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

#include "morgothdaemon.h"
#include "persistor.h"
#include "pluginmanager.h"
#include "servermanager.h"
#include "config.h"
#include <QtCore>
#include <QtDBus>

int main(int argc, char** argv)
{
    morgoth::MorgothDaemon app(argc, argv);
    app.setApplicationName(QStringLiteral("morgoth"));
    app.setApplicationVersion(QString(MORGOTH_VERSION));

    morgoth::ServerManager* servers = new morgoth::ServerManager(&app);
    app.setProperty("servers", QVariant::fromValue(servers));

    morgoth::PluginManager* plugins = new morgoth::PluginManager(&app);
    app.setProperty("plugins", QVariant::fromValue(plugins));

    // load all plugins from the build directory
    plugins->addPluginsDir(MORGOTH_BUILD_DIR);

    // handle database
    morgoth::Persistor* persistor = new morgoth::Persistor(servers, plugins, &app);
    app.setProperty("persistor", QVariant::fromValue(persistor));

    // create dbus service
    QDBusConnection dbus = app.dbusConnection();
    if (!dbus.registerService(app.dbusServiceName()))
        qFatal("Error registering service in the system bus: %s", qPrintable(dbus.lastError().message()));

    return app.exec();
}
