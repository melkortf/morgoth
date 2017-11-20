#include "servermanageradaptor.h"
#include "serveradaptor.h"
#include "../servercoordinator.h"
#include <QtCore>
#include <QtDBus>

namespace morgoth { namespace dbus {

ServerManagerAdaptor::ServerManagerAdaptor(ServerManager* serverManager) :
    QDBusAbstractAdaptor(serverManager),
    m_serverManager(serverManager)
{
    QTimer::singleShot(0, this, &ServerManagerAdaptor::registerServers);
}

ServerManagerAdaptor::~ServerManagerAdaptor()
{

}

QStringList ServerManagerAdaptor::servers() const
{
    QStringList servers;
    for (Server* s: m_serverManager->servers())
        servers << s->name();

    return servers;
}

bool ServerManagerAdaptor::add(QString path, QString name)
{
    return m_serverManager->add(path, name);
}

void ServerManagerAdaptor::registerServers()
{
    QDBusConnection dbus = QDBusConnection::systemBus();

    for (Server* server: m_serverManager->servers()) {
        new ServerAdaptor(server);
        QString path = QString("/servers/%1").arg(server->name());
        dbus.registerObject(path, server);
    }

    connect(m_serverManager, &ServerManager::serverAdded, this, &ServerManagerAdaptor::handleNewServer);
}

void ServerManagerAdaptor::handleNewServer(Server* server)
{
    new ServerAdaptor(server);
    QString path = QString("/servers/%1").arg(server->name());
    QDBusConnection::systemBus().registerObject(path, server);
}

}} // namespace morgoth::dbus
