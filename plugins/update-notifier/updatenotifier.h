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

#ifndef UPDATENOTIFIER_H
#define UPDATENOTIFIER_H

#include "morgothplugin.h"
#include "servermanager.h"
#include <QtCore/QObject>

namespace morgoth {

/**
 * \brief The UpdateNotifier plugin notifies user if there's an update
 *  available for any of the servers.
 */
class UpdateNotifier : public QObject, public MorgothPlugin {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.morgoth.MorgothPlugin" FILE "updatenotifier.json")
    Q_INTERFACES(morgoth::MorgothPlugin)

public:
    explicit UpdateNotifier(QObject *parent = nullptr);

    virtual ~UpdateNotifier();

private:
    void initialize(ServerManager* serverManager);

private slots:
    void handleServer(Server* server);

};

} // namespace morgoth

#endif // UPDATENOTIFIER_H
