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

#ifndef USERPROCESS_H
#define USERPROCESS_H

#include "morgoth_export.h"
#include <QtCore/QProcess>

namespace morgoth {

/**
 * \brief The UserProcess class executes process as a specified
 *  user.
 */
class MORGOTH_EXPORT UserProcess : public QProcess {
    Q_OBJECT

    /**
     * The user that this process is started as.
     */
    Q_PROPERTY(QString user READ user WRITE setUser)

public:
    /**
     * \brief Creates a new UserProcess instance.
     * \param parent Passed to QObject.
     */
    explicit UserProcess(QObject* parent = nullptr);

    void setUser(const QString& user);
    const QString& user() const { return m_user; }

protected:
    /**
     * \copydoc QProcess::setupChildProcess()
     */
    void setupChildProcess() override;

private:
    QString m_user;

};

} // namespace morgoth

#endif // USERPROCESS_H
