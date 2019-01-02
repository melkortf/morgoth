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

#ifndef STEAMID_H
#define STEAMID_H

#include "morgoth_export.h"
#include <QMetaType>
#include <QString>

namespace morgoth {

/**
 * \brief The SteamId class utilized the SteamID handling.
 */
class MORGOTH_EXPORT SteamId {
public:
    enum Format {
        Text    /**< STEAM_X:Y:Z */,
        Id3     /**< a.k.a. Community Id, U:1:W */,
        Id64,
    };

    /**
     * Default constructor sets steamId to 0.
     */
    SteamId();

    /**
     * Creates a new instance of \c SteamId from the provided steamId64 value.
     */
    explicit SteamId(quint64 steamId64);

    /**
     * Attempts to parse the given \c steamId with the provided \c format.
     * If this operation fails, internal steamId is set to 0.
     */
    explicit SteamId(const QString& steamId, Format format);

    quint64 toSteamId64() const { return m_steamId; }

    /**
     * \brief Converts the SteamID to the given format and returns it.
     */
    QString toFormat(Format format) const;

    bool operator==(const SteamId& other) const;

private:
    quint64 m_steamId;

};

} // namespace morgoth

Q_DECLARE_METATYPE(morgoth::SteamId);

#endif // STEAMID_H
