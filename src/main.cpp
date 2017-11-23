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
