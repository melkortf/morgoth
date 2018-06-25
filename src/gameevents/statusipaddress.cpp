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

#include "statusipaddress.h"

namespace morgoth {

StatusIpAddress::StatusIpAddress(QObject* parent) :
    GameEvent(Name, parent)
{

}

QRegularExpression StatusIpAddress::regex() const
{
    // https://regex101.com/r/Onyh45/1
    return QRegularExpression("^udp\\/ip\\s*:\\s*(\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}):(\\d{1,5}).+$");
}

void StatusIpAddress::maybeActivated(const QString& line, const QRegularExpressionMatch& match)
{
    Q_UNUSED(line);
    m_ip = match.captured(1);

    QString port = match.captured(2);
    bool ok;
    quint32 tmp = port.toUInt(&ok);
    if (ok) {
        m_port = tmp;
        emit activated();
    }
}

} // namespace morgoth
