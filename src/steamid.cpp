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

#include "steamid.h"
#include <QtCore>

namespace {

struct SteamId64 {
    uint y: 1, accountNumber: 31, instance: 20, type: 4, universe: 8;
};

static quint64 combine(quint32 y, quint32 accountNumber, quint32 accountType,
                       quint32 universe)
{
    Q_STATIC_ASSERT(sizeof(SteamId64) == sizeof(quint64));
    SteamId64 steamId = { y, accountNumber, 1, accountType, universe };
    return *reinterpret_cast<quint64*>(&steamId);
}

// https://developer.valvesoftware.com/wiki/SteamID
static quint64 fromSteamIdFormat(const QString& steamId, morgoth::SteamId::Format format)
{
    switch (format) {
        case morgoth::SteamId::Format::Id3: {
            QRegularExpression re("^\\[?(\\w):(\\d):(\\d+)\\]?$");
            auto match = re.match(steamId);
            if (match.hasMatch()) {
                uint universe = match.captured(2).toUInt();
                uint w = match.captured(3).toUInt();
                uint y = w & 0x1;
                uint z = (w - y) / 2;
                return combine(y, z, 1, universe);
            } else {
                return 0;
            }
        }

        case morgoth::SteamId::Format::Text: {
            QRegularExpression re("^STEAM_(\\d):(\\d):(\\d+)$");
            auto match = re.match(steamId);
            if (match.hasMatch()) {
                uint universe = match.captured(1).toUInt();
                if (universe == 0)
                    universe = 1; // steamID bug
                uint y = match.captured(2).toUInt();
                uint z = match.captured(3).toUInt();
                return combine(y, z, 1, universe);
            } else {
                return 0;
            }
        }

        case morgoth::SteamId::Format::Id64:
            return static_cast<quint64>(steamId.toLongLong());
    }

    return 0;
}

} // namespace

namespace morgoth {

SteamId::SteamId() : m_steamId(0)
{

}

SteamId::SteamId(quint64 steamId64) :
    m_steamId(steamId64)
{

}

SteamId::SteamId(const QString& steamId, Format format) :
    m_steamId(::fromSteamIdFormat(steamId, format))
{

}

QString SteamId::toFormat(SteamId::Format format) const
{
    switch (format) {
        case Format::Id64:
            return QString::number(m_steamId);

        case morgoth::SteamId::Format::Text: {
            const SteamId64* steamId = reinterpret_cast<const SteamId64*>(&m_steamId);
            return QString("STEAM_%1:%2:%3")
                    .arg(steamId->universe == 1 ? 0 : steamId->universe)
                    .arg(steamId->y)
                    .arg(steamId->accountNumber);
        }

        case morgoth::SteamId::Format::Id3: {
            const SteamId64* steamId = reinterpret_cast<const SteamId64*>(&m_steamId);
            auto w = steamId->accountNumber * 2 + steamId->y;
            return QString("[U:%1:%2]")
                    .arg(steamId->universe)
                    .arg(w);
        }
    }

    Q_UNREACHABLE();
}

bool SteamId::operator==(const SteamId& other) const
{
    return m_steamId == other.m_steamId;
}

} // namespace morgoth
