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

#include "userprocess.h"
#include <QtCore>
#include <sys/types.h>
#include <grp.h>
#include <pwd.h>
#include <unistd.h>

namespace morgoth {

UserProcess::UserProcess(QObject *parent) :
    QProcess(parent) {}

void UserProcess::setUser(const QString& user)
{
    m_user = user;
}

void UserProcess::setupChildProcess()
{
    qDebug() << Q_FUNC_INFO;

    if (!m_user.isEmpty()) {
        passwd* pwd = ::getpwnam(m_user.toLocal8Bit().constData());
        if (!pwd) {
            qWarning("%s; getpwnam failed", Q_FUNC_INFO);
            return;
        }

        qDebug("%s: running as %s (uid=%d, gid=%d)", Q_FUNC_INFO, qPrintable(m_user), pwd->pw_uid, pwd->pw_gid);
        ::setgroups(0, nullptr);
        ::setgid(pwd->pw_gid);
        ::setuid(pwd->pw_uid);
    }
}

} // namespace morgoth
