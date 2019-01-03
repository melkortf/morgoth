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

#include "playerinfo.h"
#include <QtDBus>

namespace morgoth {

class PlayerInfoData : public QSharedData {
public:
    PlayerInfoData(const QString& name) : name(name) {}

    QString name;
    SteamId steamId;
};

PlayerInfo::PlayerInfo(const QString& name) : d(new PlayerInfoData(name))
{

}

PlayerInfo::PlayerInfo(const PlayerInfo& other) : d(other.d)
{

}

PlayerInfo::~PlayerInfo()
{

}

PlayerInfo& PlayerInfo::operator=(const PlayerInfo& other)
{
    d = other.d;
    return *this;
}

QString PlayerInfo::name() const
{
    return d->name;
}

void PlayerInfo::setName(const QString& name)
{
    d->name = name;
}

SteamId PlayerInfo::steamId() const
{
    return d->steamId;
}

void PlayerInfo::setSteamId(const SteamId& steamId)
{
    d->steamId = steamId;
}

bool PlayerInfo::operator==(const PlayerInfo& other) const
{
    return  name() == other.name() && steamId() == other.steamId();
}

} // namespace morgoth

QDBusArgument& operator<<(QDBusArgument& argument, const morgoth::PlayerInfo& player)
{
    argument.beginStructure();
    argument << player.name();
    argument << player.steamId().toSteamId64();
    argument.endStructure();
    return argument;
}

const QDBusArgument& operator>>(const QDBusArgument& argument, morgoth::PlayerInfo& player)
{
    argument.beginStructure();

    QString name;
    quint64 steamId64;

    argument >> name;
    argument >> steamId64;

    argument.endStructure();

    player.setName(name);
    player.setSteamId(morgoth::SteamId(steamId64));

    return argument;
}

static void registerMetaType()
{
    qDBusRegisterMetaType<morgoth::PlayerInfo>();
    qDBusRegisterMetaType<morgoth::PlayerInfoList>();
}
Q_COREAPP_STARTUP_FUNCTION(registerMetaType)
