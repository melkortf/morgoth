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
#include <QtCore>
#include <algorithm>
#include <random>

namespace morgoth {

constexpr auto TmuxExec = "tmux";

TmuxSessionWrapper::TmuxSessionWrapper()
{
    findTmuxExec();
    generateRandomName();
}

TmuxSessionWrapper::~TmuxSessionWrapper()
{
    kill();
}

bool TmuxSessionWrapper::create()
{
    if (exists()) {
        qWarning("TmuxSessionWrapper::create(): the session is already created");
        return false;
    }

    int ret = QProcess::execute(m_tmuxExec, {
        "new-session",
        "-d", // detached
        "-s", name() // session name
    });

    return !ret;
}

bool TmuxSessionWrapper::redirectOutput(const QString& dest)
{
    if (!exists())
        return false;

    int ret = QProcess::execute(m_tmuxExec, {
        "pipe-pane",
        "-t", name(), // session name
        QString("/usr/bin/cat > %1").arg(dest) // redirect output to cat and cat output to dest
    });
    return !ret;
}

bool TmuxSessionWrapper::sendKeys(const QString& keys)
{
    if (!exists())
        return false;

    int ret = QProcess::execute(m_tmuxExec, {
        "send-keys",
        "-t", name(),
        keys, "C-m"
    });
    return !ret;
}

bool TmuxSessionWrapper::kill()
{
    if (!exists())
        return true;

    int ret = QProcess::execute(m_tmuxExec, {
        "kill-session",
        "-t", name()
    });

    return !ret;
}

bool TmuxSessionWrapper::exists() const
{
    QProcess tmux;
    tmux.start(m_tmuxExec, {
        "has-session",
        "-t", name()
    });

    tmux.waitForFinished();
    return tmux.exitCode() == 0;
}

void TmuxSessionWrapper::findTmuxExec()
{
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QStringList path = env.value("PATH").split(QDir::listSeparator());
    for (const QString& p: qAsConst(path)) {
        QString maybe = QDir::cleanPath(p + QDir::separator() + TmuxExec);
        if (QFile::exists(maybe)) {
            m_tmuxExec = maybe;
            break;
        }
    }

    if (m_tmuxExec.isEmpty())
        qFatal("Could not find tmux executable; exiting.");
}

void TmuxSessionWrapper::generateRandomName()
{
    m_name = "morgoth-" + []() -> QString {
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

} // namespace Morgoth
