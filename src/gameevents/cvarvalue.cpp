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

#include "cvarvalue.h"

namespace {
QString produceName(const QString& cvar)
{
    return QStringLiteral("cvar.%1").arg(cvar);
}
}

namespace morgoth {

CvarValue::CvarValue(QString cvar, QObject* parent) :
    GameEvent(::produceName(cvar), parent),
    m_name(cvar)
{

}

QRegularExpression CvarValue::regex() const
{
    // https://regex101.com/r/9bAKzI/3
    return QRegularExpression(
        QStringLiteral("^\\\"%1\\\"\\s?=\\s?\\\"(.*)\\\"\\s?(?:\\(\\s?def\\.\\s?\\\"(.*)\\\"\\s?\\))?$").arg(m_name)
                );
}

void CvarValue::maybeActivated(const QString& line, const QRegularExpressionMatch& match)
{
    Q_UNUSED(line);
    m_value = match.captured(1);
    m_defaultValue = match.captured(2);
    emit activated();
}

} // namespace morgoth
