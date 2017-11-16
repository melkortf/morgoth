#include "servermanageradaptor.h"

namespace morgoth { namespace dbus {

ServerManagerAdaptor::ServerManagerAdaptor(ServerManager* serverManager) :
    QDBusAbstractAdaptor(serverManager)
{

}

ServerManagerAdaptor::~ServerManagerAdaptor()
{

}

}} // namespace morgoth::dbus
