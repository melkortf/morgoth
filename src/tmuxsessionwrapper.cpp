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

#include "tmuxsessionwrapper.h"
#include "userprocess.h"
#include <QtCore>
#include <algorithm>
#include <random>
#include <memory>
#include <pwd.h>
#include <sys/types.h>

namespace {
static constexpr int TmuxProcessTimeout = 1000; // 1 second

class TmuxProcessFactory {
private:
    // FIXME Share the executable
    static QString tmuxExec() {
        constexpr auto TmuxExec = "tmux";
        static QString tmuxBin;

        if (tmuxBin.isEmpty()) {
            QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
            QStringList path = env.value("PATH").split(QDir::listSeparator());
            for (const QString& p: qAsConst(path)) {
                QString maybe = QDir::cleanPath(p + QDir::separator() + TmuxExec);
                if (QFile::exists(maybe)) {
                    tmuxBin = maybe;
                    break;
                }
            }

            if (tmuxBin.isEmpty())
                qFatal("Could not find tmux executable; exiting.");
        }

        return tmuxBin;
    }

public:
    std::unique_ptr<morgoth::UserProcess> create(const QString& user)
    {
        using morgoth::UserProcess;
        std::unique_ptr<UserProcess> process = std::make_unique<UserProcess>();
        process->setProgram(tmuxExec());
        process->setUser(user);
        return process;
    }
};

QString generateRandomSessionName()
{
    return QString("morgoth-") % []() -> QString {
        auto randchar = []() -> char {
            const char charset[] = "0123456789abcdef";
            const int max_index = (sizeof(charset) - 2);

            std::random_device rd;
            std::mt19937 rng(rd());
            std::uniform_int_distribution<size_t> uni(0, max_index);

            return charset[uni(rng)];
        };

        const int length = 6;
        QString str(length, '-');
        std::generate(str.begin(), str.end(), randchar);
        return str;
    }();
}
}

namespace morgoth {

class TmuxSessionWrapperPrivate {
public:
    QSharedPointer<TmuxProcessFactory> tmuxFactory { new TmuxProcessFactory };
    QString name;
    QString user;
};


TmuxSessionWrapper::TmuxSessionWrapper() :
    d(new TmuxSessionWrapperPrivate)
{
    d->name = ::generateRandomSessionName();
}

TmuxSessionWrapper::~TmuxSessionWrapper()
{
    kill();
}

bool TmuxSessionWrapper::create()
{
    if (exists()) {
        qWarning("%s: the session is already created", Q_FUNC_INFO);
        return false;
    }

    auto tmux = d->tmuxFactory->create(d->user);
    tmux->setArguments({
        "new-session",
        "-d", // detached
        "-s", name() // session name
    });

    tmux->start();
    bool result = tmux->waitForFinished(TmuxProcessTimeout)
            && tmux->exitStatus() == QProcess::ExitStatus::NormalExit
            && tmux->exitCode() == 0;

    if (!result) {
        qWarning() << Q_FUNC_INFO << tmux->readAllStandardOutput();
        qWarning() << Q_FUNC_INFO << tmux->readAllStandardError();
    }

    return result;
}

bool TmuxSessionWrapper::redirectOutput(const QString& dest)
{
    if (!exists()) {
        qWarning("%s: the session is not created", Q_FUNC_INFO);
        return false;
    }

    auto tmux = d->tmuxFactory->create(d->user);
    tmux->setArguments({
        "pipe-pane",
        "-t", name(), // session name
        QString("/usr/bin/cat > %1").arg(dest) // redirect output to cat and cat output to dest
    });

    tmux->start();
    bool result = tmux->waitForFinished(TmuxProcessTimeout)
            && tmux->exitStatus() == QProcess::ExitStatus::NormalExit
            && tmux->exitCode() == 0;

    if (!result) {
        qWarning() << Q_FUNC_INFO << tmux->readAllStandardOutput();
        qWarning() << Q_FUNC_INFO << tmux->readAllStandardError();
    }

    return result;
}

bool TmuxSessionWrapper::sendKeys(const QString& keys)
{
    if (!exists()) {
        qWarning("%s: the session is not created", Q_FUNC_INFO);
        return false;
    }

    auto tmux = d->tmuxFactory->create(d->user);
    tmux->setArguments({
        "send-keys",
        "-t", name(),
        keys, "C-m"
    });

    tmux->start();
    bool result = tmux->waitForFinished(TmuxProcessTimeout)
            && tmux->exitStatus() == QProcess::ExitStatus::NormalExit
            && tmux->exitCode() == 0;

    if (!result) {
        qWarning() << Q_FUNC_INFO << tmux->readAllStandardOutput();
        qWarning() << Q_FUNC_INFO << tmux->readAllStandardError();
    }

    return result;
}

bool TmuxSessionWrapper::kill()
{
    if (!exists())
        return true;

    auto tmux = d->tmuxFactory->create(d->user);
    tmux->setArguments({
        "kill-session",
        "-t", name()
    });

    tmux->start();
    bool result = tmux->waitForFinished(TmuxProcessTimeout)
            && tmux->exitStatus() == QProcess::ExitStatus::NormalExit
            && tmux->exitCode() == 0;

    if (!result) {
        qWarning() << Q_FUNC_INFO << tmux->readAllStandardOutput();
        qWarning() << Q_FUNC_INFO << tmux->readAllStandardError();
    }

    return result;
}

const QString& TmuxSessionWrapper::name() const
{
    return d->name;
}

bool TmuxSessionWrapper::exists() const
{
    auto tmux = d->tmuxFactory->create(d->user);
    tmux->setArguments({
        "has-session",
        "-t", name()
    });

    tmux->start();

    if (!tmux->waitForFinished(TmuxProcessTimeout)) {
        qWarning() << Q_FUNC_INFO << tmux->readAllStandardOutput();
        qWarning() << Q_FUNC_INFO << tmux->readAllStandardError();
    }

    return tmux->exitStatus() == QProcess::ExitStatus::NormalExit
            && tmux->exitCode() == 0;
}

const QString TmuxSessionWrapper::user() const
{
    return d->user;
}

void TmuxSessionWrapper::setUser(const QString& user)
{
    if (user != d->user) {
        if (exists()) {
            qWarning("%s: cannot change the user while the session is running", Q_FUNC_INFO);
            return;
        }

        d->user = user;
    }
}

} // namespace Morgoth
