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

#ifndef MORGOTHCTLPLUGIN_H
#define MORGOTHCTLPLUGIN_H

#include <QtPlugin>
#include <QtCore/QString>
#include <QtCore/QTextStream>
#include <QtDBus/QDBusConnection>

namespace morgoth {

/**
 * \brief MorgothCtlPlugin is a plugins that extends \c morgothctl usage.
 */
class MorgothCtlPlugin {
public:
    /**
     * \brief Returns command that this plugin handles.
     */
    virtual QString command() const = 0;

    /**
     * \brief Handles this plugin execution.
     * \param dbus The D-Bus connection to the daemon.
     * \param arguments Argument passed by the user to \c morgothctl.
     * \param out Output stream, most likely \c stdout.
     * \return The return code; 0 for success.
     */
    virtual int execute(QDBusConnection dbus, const QStringList& arguments, QTextStream& out) = 0;

    virtual ~MorgothCtlPlugin() = default;

};

} // namespace morgoth

#endif // MORGOTHCTLPLUGIN_H
