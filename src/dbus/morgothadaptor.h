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

#ifndef MORGOTHADAPTOR_H
#define MORGOTHADAPTOR_H

#include <QtCore/QCoreApplication>
#include <QtDBus/QDBusAbstractAdaptor>

namespace morgoth { namespace dbus {

/**
 * \brief The MorgothAdaptor class is a D-Bus adaptor for the morgoth
 * daemon.
 */
class MorgothAdaptor : public QDBusAbstractAdaptor {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.morgoth.Morgoth")
    Q_PROPERTY(QString version READ version)

public:
    explicit MorgothAdaptor(QCoreApplication* app);

    QString version() const;

public slots:
    Q_NOREPLY void quit();

private:
    QCoreApplication* m_app;

};

}} // namespace morgoth::dbus

#endif // MORGOTHADAPTOR_H
