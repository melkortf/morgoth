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

#include "configuration.h"
#include "morgothdaemon.h"
#include "servermanager.h"
#include "dbus/servermanageradaptor.h"
#include "config.h"
#include <QtCore>
#include <QtDBus>

using namespace morgoth;

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    app.setApplicationVersion(QString(MORGOTH_VERSION));

    QCommandLineParser parser;
    parser.addVersionOption();

    QCommandLineOption configFileOption(QStringList() << "c" << "config", "Config file to use", "config");
    parser.addOption(configFileOption);

    parser.process(app);

    QString configFile = parser.isSet(configFileOption) ? parser.value(configFileOption) : QString();
    Configuration* config = new Configuration(configFile);
    app.setProperty("configuration", QVariant::fromValue(config));

    // make this application a good daemon
    MorgothDaemon* daemon = new MorgothDaemon(&app);

    ServerManager* sm = new ServerManager(&app);
    app.setProperty("serverManager", QVariant::fromValue(sm));

    QDBusConnection dbus = daemon->dbusConnection();
    if (!dbus.registerService("org.morgoth"))
        qFatal("Error registering service in the system bus: %s", qPrintable(dbus.lastError().message()));

    return app.exec();
}
