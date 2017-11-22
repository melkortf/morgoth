#ifndef MORGOTHDAEMON_H
#define MORGOTHDAEMON_H

#include <QtCore/QObject>
#include <QtCore/QSocketNotifier>
#include <QtDBus/QDBusConnection>

namespace morgoth {

/**
 * @brief The MorgothDaemon class makes morgoth a proper posix daemon.
 */
class MorgothDaemon : public QObject {
    Q_OBJECT

public:
    MorgothDaemon(QObject* parent = nullptr);

    QDBusConnection dbusConnection() const { return m_dbusConnection; }

private slots:
    void handleSignal();

private:
    QSocketNotifier* m_signal;
    QDBusConnection m_dbusConnection;

};

} // namespace morgoth

#endif // MORGOTHDAEMON_H
