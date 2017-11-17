#include "serveradaptor.h"
#include <QtCore>

namespace morgoth { namespace dbus {

ServerAdaptor::ServerAdaptor(Server* server) :
    QDBusAbstractAdaptor(server),
    m_server(server)
{
    connect(m_server->coordinator(), &ServerCoordinator::statusChanged,
            this, &ServerAdaptor::handleServerStatusChange);
}

QString ServerAdaptor::name() const
{
    return m_server->name();
}

QString ServerAdaptor::path() const
{
    return m_server->path();
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
