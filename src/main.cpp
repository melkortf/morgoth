#include "morgothdaemon.h"
#include "servermanager.h"
#include "dbus/morgothadaptor.h"
#include "dbus/servermanageradaptor.h"
#include <QtCore>
#include <QtDBus>

using namespace morgoth;

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    app.setApplicationVersion("0.1");

    // handle unix signals, as a good daemon should do
    new MorgothDaemon(&app);

    // setup D-Bus daemon so one can control the daemon
    QDBusConnection dbus = QDBusConnection::systemBus();

    new dbus::MorgothAdaptor(&app);
    dbus.registerObject("/daemon", &app);

    ServerManager* sm = new ServerManager(&app);
    new dbus::ServerManagerAdaptor(sm);
    dbus.registerObject("/servers", sm);

    if (!dbus.registerService("org.morgoth"))
        qFatal("Error registering service in the system bus: %s", qPrintable(dbus.lastError().message()));

    return app.exec();
}
