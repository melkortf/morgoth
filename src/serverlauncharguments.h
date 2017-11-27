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

#ifndef SERVERLAUNCHARGUMENTS_H
#define SERVERLAUNCHARGUMENTS_H

#include <QtCore/QMetaType>
#include <QtCore/QSharedData>
#include <QtCore/QString>

namespace morgoth {

class ServerLaunchArgumentsData;

class ServerLaunchArguments {
public:
    ServerLaunchArguments();
    ServerLaunchArguments(const ServerLaunchArguments& other);
    virtual ~ServerLaunchArguments();

    ServerLaunchArguments& operator=(const ServerLaunchArguments& other);

    unsigned port() const;
    void setPort(unsigned port);
    bool isSecured() const;
    void setSecured(bool secured);
    QString initialMap() const;
    void setInitialMap(const QString& initialMap);

    QString asSrcdsArguments() const;

private:
    QSharedDataPointer<ServerLaunchArgumentsData> d;

};

} // namespace morgoth

Q_DECLARE_METATYPE(morgoth::ServerLaunchArguments)
Q_DECLARE_TYPEINFO(morgoth::ServerLaunchArguments, Q_MOVABLE_TYPE);

#endif // SERVERLAUNCHARGUMENTS_H
