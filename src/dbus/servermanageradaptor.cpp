#include "servermanageradaptor.h"
#include "serveradaptor.h"
#include "../servercoordinator.h"
#include <QtCore>
#include <QtDBus>

namespace morgoth { namespace dbus {

ServerManagerAdaptor::ServerManagerAdaptor(ServerManager* serverManager) :
    QDBusAbstractAdaptor(serverManager),
    m_serverManager(serverManager),
    m_morgothDaemon(qApp->property("daemon").value<MorgothDaemon*>())
{
    Q_ASSERT(m_morgothDaemon);
    m_morgothDaemon->dbusConnection().registerObject("/servers", serverManager);

    connect(m_serverManager, &ServerManager::serverAdded, this, &ServerManagerAdaptor::handleNewServer);
}

ServerManagerAdaptor::~ServerManagerAdaptor() {}

QStringList ServerManagerAdaptor::servers() const
{
    QStringList servers;
    for (Server* s: m_serverManager->servers())
        servers << s->name();

    return servers;
}

bool ServerManagerAdaptor::add(QString path, QString name)
{
    return m_serverManager->add(QUrl::fromLocalFile(path), name) != nullptr;
}

void ServerManagerAdaptor::handleNewServer(Server* server)
{
    emit serverAdded(server->name());
}

}} // namespace morgoth::dbus
