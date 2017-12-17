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
    if (m_user != user) {
        m_user = user;
        resolveGidAndUid();
    }
}

void UserProcess::setupChildProcess()
{
    if (m_userResolved) {
        ::setgroups(0, nullptr);
        ::setgid(m_gid);
        ::setuid(m_uid);
    }
}

void UserProcess::resolveGidAndUid()
{
    if (!m_user.isEmpty()) {
        passwd* pwd = ::getpwnam(m_user.toLocal8Bit().constData());
        if (!pwd) {
            char* error = ::strerror(errno);
            qCritical("Error retrieving uid and gid of user %s (%s)", qPrintable(user()), error);
        } else {
            m_gid = pwd->pw_gid;
            m_uid = pwd->pw_uid;
            m_userResolved = true;
        }
    }
}

} // namespace morgoth
