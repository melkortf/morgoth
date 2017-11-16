#ifndef SERVERMANAGERADAPTOR_H
#define SERVERMANAGERADAPTOR_H

#include "../servermanager.h"
#include <QtDBus/QDBusAbstractAdaptor>

namespace morgoth { namespace dbus {

class ServerManagerAdaptor : public QDBusAbstractAdaptor {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.eu.morgoth.ServerManager")

public:
    explicit ServerManagerAdaptor(ServerManager* serverManager);

    virtual ~ServerManagerAdaptor();

};

}} // namespace morgoth::dbus

#endif // SERVERMANAGERADAPTOR_H
