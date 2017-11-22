#ifndef SERVERADAPTOR_H
#define SERVERADAPTOR_H

#include "../server.h"
#include "../servercoordinator.h"
#include <QtDBus/QDBusAbstractAdaptor>

namespace morgoth { class MorgothDaemon; }

namespace morgoth { namespace dbus {

class ServerAdaptor : public QDBusAbstractAdaptor {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.morgoth.Server")
    Q_PROPERTY(QString name READ name)
    Q_PROPERTY(QString path READ path)
    Q_PROPERTY(bool valid READ isValid)
    Q_PROPERTY(QString status READ status NOTIFY statusChanged)

signals:
    void statusChanged(QString newStatus);

public:
    explicit ServerAdaptor(Server* server);

    QString name() const;
    QString path() const;
    bool isValid() const;
    QString status() const;

public slots:
    Q_NOREPLY void start();
    Q_NOREPLY void stop();

private slots:
    void handleServerStatusChange(ServerCoordinator::Status status);

private:
    Server* m_server;
    MorgothDaemon* m_morgothDaemon;

};

}} // namespace morgoth::dbus

#endif // SERVERADAPTOR_H
