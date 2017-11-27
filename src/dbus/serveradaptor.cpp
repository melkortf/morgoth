#include "serveradaptor.h"
#include "../morgothdaemon.h"
#include <QtCore>
#include <QtDBus>

namespace morgoth { namespace dbus {

ServerAdaptor::ServerAdaptor(Server* server) :
    QDBusAbstractAdaptor(server),
    m_server(server),
    m_morgothDaemon(qApp->property("daemon").value<MorgothDaemon*>())
{
    connect(m_server->coordinator(), &ServerCoordinator::statusChanged,
            this, &ServerAdaptor::handleServerStatusChange);

    QString path = QString("/servers/%1").arg(m_server->name());
    QDBusConnection dbus = m_morgothDaemon->dbusConnection();
    dbus.registerObject(path, m_server);
}

QString ServerAdaptor::name() const
{
    return m_server->name();
}

QString ServerAdaptor::path() const
{
    return m_server->path().toString();
}

bool ServerAdaptor::isValid() const
{
    return m_server->isValid();
}

QString ServerAdaptor::status() const
{
    return QMetaEnum::fromType<ServerCoordinator::Status>().valueToKey(m_server->coordinator()->status());
}

void ServerAdaptor::start()
{
    m_server->coordinator()->start();
}

void ServerAdaptor::stop()
{
    m_server->coordinator()->stop();
}

void ServerAdaptor::handleServerStatusChange(ServerCoordinator::Status status)
{
    Q_UNUSED(status);
    emit statusChanged(this->status());
}

}} // namespace morgoth::dbus
