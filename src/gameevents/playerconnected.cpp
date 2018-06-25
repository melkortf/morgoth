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

#include "playerconnected.h"

namespace morgoth {

PlayerConnected::PlayerConnected(QObject* parent) :
    GameEvent(Name, parent)
{
    
}

QRegularExpression PlayerConnected::regex() const
{
    // https://regex101.com/r/uyPW8m/2
    return QRegularExpression(
                "^\\w\\s(\\d{2}\\/\\d{2}\\/\\d{4})\\s-\\s(\\d{2}"
                ":\\d{2}:\\d{2}):\\s\\\"(.[^\\\"]+)\"\\sconnected,"
                "\\saddress\\s\\\"(\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}"
                "\\.\\d{1,3}:\\d{1,5})\"$");
}

} // namespace morgoth
