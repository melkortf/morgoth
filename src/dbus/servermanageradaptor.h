#ifndef SERVERMANAGERADAPTOR_H
#define SERVERMANAGERADAPTOR_H

#include "../servermanager.h"
#include <QtCore/QStringList>
#include <QtDBus/QDBusAbstractAdaptor>

namespace morgoth { namespace dbus {

class ServerManagerAdaptor : public QDBusAbstractAdaptor {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.morgoth.ServerManager")
    Q_PROPERTY(QStringList servers READ servers)

signals:
    void serverAdded(const QString& serverName);

public:
    explicit ServerManagerAdaptor(ServerManager* serverManager);

    virtual ~ServerManagerAdaptor();

    QStringList servers() const;

public slots:
    bool add(QString path, QString name);

private:
    void registerServers();

private slots:
    void handleNewServer(Server* server);

private:
    ServerManager* m_serverManager;

};

}} // namespace morgoth::dbus

#endif // SERVERMANAGERADAPTOR_H
