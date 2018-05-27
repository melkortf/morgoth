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

class UserProcessPrivate {
public:
    void resolveGidAndUid()
    {
        if (!user.isEmpty()) {
            passwd* pwd = ::getpwnam(user.toLocal8Bit().constData());
            if (!pwd) {
                char* error = ::strerror(errno);
                qCritical("Error retrieving uid and gid of user %s (%s)", qPrintable(user), error);
            } else {
                gid = pwd->pw_gid;
                uid = pwd->pw_uid;
                userResolved = true;
            }
        }
    }

    QString user;
    unsigned int gid = 0;
    unsigned int uid = 0;
    bool userResolved = false;
};

UserProcess::UserProcess(QObject *parent) :
    QProcess(parent),
    d(new UserProcessPrivate)
{

}

UserProcess::~UserProcess()
{

}

void UserProcess::setUser(const QString& user)
{
    if (d->user != user) {
        d->user = user;
        d->resolveGidAndUid();
    }
}

const QString& UserProcess::user() const
{
    return d->user;
}

unsigned int UserProcess::groupId() const
{
    return d->gid;
}

unsigned int UserProcess::userId() const
{
    return d->uid;
}

void UserProcess::setupChildProcess()
{
    if (d->userResolved) {
        ::setgroups(0, nullptr);
        ::setgid(d->gid);
        ::setuid(d->uid);
    }
}

} // namespace morgoth
