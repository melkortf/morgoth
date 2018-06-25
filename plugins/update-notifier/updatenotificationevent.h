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

#ifndef UPDATENOTIFICATIONEVENT_H
#define UPDATENOTIFICATIONEVENT_H

#include "gameevent.h"

namespace morgoth {

class UpdateNotificationEvent : public GameEvent {
    Q_OBJECT

public:
    explicit UpdateNotificationEvent(QObject *parent = nullptr);

    QRegularExpression regex() const override;

    static auto constexpr Name = "updatenotification";

};

} // namespace morgoth

#endif // UPDATENOTIFICATIONEVENT_H
