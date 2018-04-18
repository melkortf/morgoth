// This file is part of morgoth.

// morgoth is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef MORGOTHDAEMON_H
#define MORGOTHDAEMON_H

#include "morgoth_export.h"
#include <QtCore/QCoreApplication>
#include <QtCore/QSocketNotifier>
#include <QtDBus/QDBusConnection>

namespace morgoth {

class MorgothDaemonPrivate;

/**
 * \brief The MorgothDaemon class makes morgoth a proper posix daemon out of
 *  a regular \c QCoreApplication.
 */
class MORGOTH_EXPORT MorgothDaemon : public QCoreApplication {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.morgoth.Daemon")

    /**
     * Stores the MorgothDaemon version.
     */
    Q_PROPERTY(QString version READ version CONSTANT)

signals:
    void configRead();

public:
    /**
     * \brief Creates a new \c MorgothDaemon instance, takes control over
     *  \c QCoreApplication.
     */
    MorgothDaemon(int& argc, char** argv);

    /**
     * Frees this \c MorgothDaemon instance.
     */
    virtual ~MorgothDaemon();

    /**
     * \brief Returns application-wide dbus connection.
     */
    QDBusConnection dbusConnection() const;

    /**
     * \brief Returns daemon config.
     */
    const QVariantMap& config() const;

    QString version() const;

    /**
     * Name of the service which morgothd should register.
     */
    static inline const char* dbusServiceName() { return "org.morgoth"; }

private slots:
    void handleSignal();

private:
    QScopedPointer<MorgothDaemonPrivate> d;

};

} // namespace morgoth

#define morgothd qobject_cast<morgoth::MorgothDaemon*>(QCoreApplication::instance())

#endif // MORGOTHDAEMON_H
