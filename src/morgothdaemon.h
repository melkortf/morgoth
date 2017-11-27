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

#include <QtCore/QObject>
#include <QtCore/QSocketNotifier>
#include <QtDBus/QDBusConnection>

namespace morgoth {

/**
 * \brief The MorgothDaemon class makes morgoth a proper posix daemon.
 * \todo Derive MorgothDaemon from QCoreApplication.
 */
class MorgothDaemon : public QObject {
    Q_OBJECT

public:
    /**
     * \brief Creates a new \c MorgothDaemon instance.
     * \param parent Passed to QObject.
     * \warning Only one instance of \c MorgothDaemon per \c QCoreApplication
     *  can be created.
     */
    MorgothDaemon(QObject* parent = nullptr);

    /**
     * \brief Returns application-wide dbus connection.
     */
    QDBusConnection dbusConnection() const { return m_dbusConnection; }

private slots:
    void handleSignal();

private:
    QSocketNotifier* m_signal;
    QDBusConnection m_dbusConnection;

};

} // namespace morgoth

#endif // MORGOTHDAEMON_H
