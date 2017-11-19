#include "tmuxsessionwrapper.h"
#include <QtCore>
#include <algorithm>
#include <random>
#include <sys/types.h>
#include <unistd.h>

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
    QString cmd = QString("%1 new -d -s %2").arg(m_tmuxExec, name());

    int ret = system(qPrintable(cmd));
    if (!ret)
        m_open = true;

    return !ret;
}

bool TmuxSessionWrapper::redirectOutput(const QString& dest)
{
    if (!m_open)
        return false;

    QString cmd = QString("%1 pipe-pane -t %2 '/usr/bin/cat > %3'").arg(m_tmuxExec, name(), dest);

    int ret = system(qPrintable(cmd));
    return !ret;
}

bool TmuxSessionWrapper::sendKeys(const QString& keys)
{
    if (!m_open)
        return false;

    QString cmd = QString("%1 send-keys -t %2 '%3' C-m").arg(m_tmuxExec, name(), keys);

    int ret = system(qPrintable(cmd));
    return !ret;
}

bool TmuxSessionWrapper::kill()
{
    if (!m_open)
        return false;

    QString cmd = QString("%1 kill-session -t %2").arg(m_tmuxExec, name());

    int ret = system(qPrintable(cmd));
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
    m_name = "morgoth_" + []() -> QString {
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
