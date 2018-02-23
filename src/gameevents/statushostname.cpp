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

#include "statushostname.h"

namespace morgoth {

StatusHostname::StatusHostname(QObject* parent) :
    EventHandler(Name, parent)
{

}

QRegularExpression StatusHostname::regex() const
{
    return QRegularExpression("^hostname\\s*\\:\\s+(.+)$");
}

void StatusHostname::maybeActivated(const QString& line, const QRegularExpressionMatch& match)
{
    Q_UNUSED(line);
    m_hostname = match.captured(1);
    emit activated();
}

} // namespace morgoth