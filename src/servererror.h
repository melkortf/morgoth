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

#ifndef SERVERERROR_H
#define SERVERERROR_H

#include "morgoth_export.h"
#include <QtCore/QString>
#include <QtDBus/QDBusArgument>

namespace morgoth {

class MORGOTH_EXPORT ServerError {
    Q_GADGET

public:
    /**
     * \brief The ErrorType enum describes an error that could cause the server
     * to crash.
     */
    enum ErrorType {
        NoError,
        InvalidInstallation,
        InvalidConfiguration,
        UnableToCreateTmuxSession,
        UnableToResolveUser,
        UnableToRedirectTmuxOutput,
        ServerExecutableFailed,
        ServerNotRegisteredOnTime,
    };
    Q_ENUM(ErrorType)

    /**
     * Creates a new ServerError instance with the given \c type.
     */
    ServerError(ErrorType type = ErrorType::NoError);

    /**
     * Returns the error type of this error.
     */
    ErrorType type() const { return m_type; }

    /**
     * Returns a human-readable string that describes the error.
     */
    QString message() const { return errorString(m_type); }

    /**
     * Compares \c other error to this one.
     * @{
     */
    bool operator ==(const ServerError& other) const { return other.m_type == m_type; }
    bool operator !=(const ServerError& other) const { return other.m_type != m_type; }
    /** @} */

    /**
     * Returns a human-readable string that describes \c error.
     */
    static QString errorString(ErrorType error);

private:
    ErrorType m_type;

};

} // namespace Morgoth

MORGOTH_EXPORT QDBusArgument& operator<<(QDBusArgument& argument, const morgoth::ServerError& error);
MORGOTH_EXPORT const QDBusArgument& operator>>(const QDBusArgument& argument, morgoth::ServerError& error);
Q_DECLARE_METATYPE(morgoth::ServerError)

#endif // SERVERERROR_H
