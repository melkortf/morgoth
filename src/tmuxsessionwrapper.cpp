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
    if (m_open) {
        qWarning("TmuxSessionWrapper::create(): the session is already created");
        return false;
    }

    int ret = QProcess::execute(m_tmuxExec, {
        "new-session",
        "-d", // detached
        "-s", name() // session name
    });

    return m_open = !ret;
}

bool TmuxSessionWrapper::redirectOutput(const QString& dest)
{
    if (!m_open)
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
    if (!m_open)
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
    if (!m_open)
        return false;

    int ret = QProcess::execute(m_tmuxExec, {
        "kill-session",
        "-t", name()
    });
    if (ret)
        m_open = false;

    return !ret;
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

    qDebug("Using tmux executable: %s", qPrintable(m_tmuxExec));
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
