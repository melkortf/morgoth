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

#include "restartcommand.h"
#include "startcommand.h"
#include "stopcommand.h"
#include <QtCore>

int RestartCommand::execute(QDBusConnection dbus, const QStringList& arguments, QTextStream& out)
{
    StopCommand stop;
    int res = stop.execute(dbus, arguments, out);
    if (res == 0) {
        StartCommand start;
        return start.execute(dbus, arguments, out);
    } else {
        return res;
    }
}
