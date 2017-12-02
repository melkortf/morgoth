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

#ifndef PERSISTOR_H
#define PERSISTOR_H

#include "morgoth_export.h"
#include "servermanager.h"
#include <QtSql/QSqlDatabase>

namespace morgoth {

/**
 * \brief The Persistor class is responsible for storing all servers and their
 *  configs.
 */
class Persistor : public QObject {
    Q_OBJECT

public:
    explicit Persistor(ServerManager* serverManager = nullptr);

private:
    void initializeDatabase();
    void restoreServers();

private slots:
    void storeServer(Server* server);

private:
    ServerManager* m_serverManager;
    QSqlDatabase m_database;

};

} // namespace morgoth

#endif // PERSISTOR_H
