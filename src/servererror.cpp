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

#include "servererror.h"
#include <QtCore>
#include <QtDBus>

namespace morgoth {

ServerError::ServerError(ErrorType type) :
    m_type(type)
{

}

QString ServerError::errorString(ServerError::ErrorType error)
{
    switch (error) {
        case ServerError::NoError:
            return "no error";
        case ServerError::InvalidInstallation:
            return "invalid installation";
        case ServerError::InvalidConfiguration:
            return "invalid configuration";
        case ServerError::UnableToCreateTmuxSession:
            return "unable to create tmux session";
        case ServerError::UnableToResolveUser:
            return "unable to resolve user";
        case ServerError::UnableToRedirectTmuxOutput:
            return "unable to redirect tmux output";
        case ServerError::ServerExecutableFailed:
            return "server executable failed";
        case ServerError::ServerNotRegisteredOnTime:
            return "server not registered on time";
        default:
            return QString();
    }
}

} // namespace Morgoth

QDBusArgument& operator<<(QDBusArgument& argument, const morgoth::ServerError& error)
{
    QString strError = QMetaEnum::fromType<morgoth::ServerError::ErrorType>().valueToKey(error.type());

    argument.beginStructure();
    argument << strError;
    argument.endStructure();

    return argument;
}

const QDBusArgument& operator>>(const QDBusArgument& argument, morgoth::ServerError& error)
{
    QString strError;

    argument.beginStructure();
    argument >> strError;
    argument.endStructure();

    int value = QMetaEnum::fromType<morgoth::ServerError::ErrorType>().keyToValue( strError.toLocal8Bit().constData());
    error = morgoth::ServerError(static_cast<morgoth::ServerError::ErrorType>(value));
    return argument;
}

static void registerMetaType()
{
    qRegisterMetaType<morgoth::ServerError>();
    qDBusRegisterMetaType<morgoth::ServerError>();
}
Q_COREAPP_STARTUP_FUNCTION(registerMetaType)
