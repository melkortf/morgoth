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

#ifndef STATUSPLAYERNUMBERS_H
#define STATUSPLAYERNUMBERS_H

#include "gameevent.h"
#include <QtCore/QObject>

namespace morgoth {

class MORGOTH_EXPORT StatusPlayerNumbers : public GameEvent {
public:
    StatusPlayerNumbers(QObject* parent = nullptr);

    QRegularExpression regex() const override;

    int playerCount() const { return m_playerCount; }
    int maxPlayers() const { return m_maxPlayers; }

    static auto constexpr Name = "status.line.players";

protected:
    void maybeActivated(const QString& line, const QRegularExpressionMatch& match) override;

private:
    int m_playerCount;
    int m_maxPlayers;

};

} // namespace morgoth

#endif // STATUSPLAYERNUMBERS_H
