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

#ifndef PLAYERINFO_H
#define PLAYERINFO_H

#include "morgoth_export.h"
#include "steamid.h"
#include <QtCore/QMetaType>
#include <QtCore/QSharedDataPointer>
#include <QtDBus/QDBusArgument>

namespace morgoth {

class PlayerInfoData;

/**
 * \brief The PlayerInfo class holds data of a player that is connected to
 *  a server.
 */
class MORGOTH_EXPORT PlayerInfo {
public:
    /**
     * \brief Creates a new instance of PlayerInfo with the given \c name.
     * \param name The nickname of the player.
     */
    explicit PlayerInfo(const QString& name = QString());

    /**
     * \brief Constructs a new instance of PlayerInfo that is a copy of \c other.
     */
    PlayerInfo(const PlayerInfo& other);

    /**
     * \brief Destroys this instance.
     */
    virtual ~PlayerInfo();

    /**
     * \brief Assigns \c other to \c this.
     */
    PlayerInfo& operator=(const PlayerInfo& other);

    QString name() const;
    void setName(const QString& name);

    SteamId steamId() const;
    void setSteamId(const SteamId& steamId);

    bool operator==(const PlayerInfo& other) const;

private:
    QSharedDataPointer<PlayerInfoData> d;

};

typedef QList<PlayerInfo> PlayerInfoList;

} // namespace morgoth

Q_DECLARE_METATYPE(morgoth::PlayerInfo);
Q_DECLARE_METATYPE(morgoth::PlayerInfoList);
MORGOTH_EXPORT QDBusArgument& operator<<(QDBusArgument& argument, const morgoth::PlayerInfo& player);
MORGOTH_EXPORT const QDBusArgument& operator>>(const QDBusArgument& argument, morgoth::PlayerInfo& player);

#endif // PLAYERINFO_H
