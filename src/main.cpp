#include "consoleinterface.h"
#include "servermanager.h"
#include "dbus/morgothadaptor.h"
#include "dbus/servermanageradaptor.h"
#include <QtCore>
#include <QtDBus>
#include <csignal>

using namespace morgoth;

void handleUnixSignals(std::initializer_list<int> quitSignals)
{
    auto handler = [](int sig) -> void {
        // blocking and not aysnc-signal-safe func are valid
        qInfo("\n--- Caught signal %d; exiting gracefully ---\n", sig);
        QCoreApplication::quit();
    };

    sigset_t blocking_mask;
    sigemptyset(&blocking_mask);
    for (auto sig: quitSignals)
        sigaddset(&blocking_mask, sig);

    struct sigaction sa;
    sa.sa_handler = handler;
    sa.sa_mask    = blocking_mask;
    sa.sa_flags   = 0;

    for (auto sig: quitSignals)
        sigaction(sig, &sa, nullptr);
}

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    app.setApplicationVersion("0.1");

    handleUnixSignals({ SIGQUIT, SIGINT, SIGTERM, SIGHUP });

    QDBusConnection dbus = QDBusConnection::sessionBus();
    dbus.registerService("org.morgoth");

    new dbus::MorgothAdaptor(&app);
    dbus.registerObject("/daemon", &app);

    ServerManager* sm = new ServerManager(&app);
    new dbus::ServerManagerAdaptor(sm);
    dbus.registerObject("/servers", sm);

    new ConsoleInterface(sm, &app);

    return app.exec();
}
