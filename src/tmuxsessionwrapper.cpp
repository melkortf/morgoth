#include "tmuxsessionwrapper.h"
#include <QtCore>
#include <algorithm>
#include <random>
#include <sys/types.h>
#include <unistd.h>

namespace Morgoth {

constexpr auto TmuxExec = "/usr/bin/tmux";

TmuxSessionWrapper::TmuxSessionWrapper()
{
    generateRandomName();
}

TmuxSessionWrapper::~TmuxSessionWrapper()
{
    kill();
}

bool TmuxSessionWrapper::create()
{
    QString cmd = QString("%1 new -d -s %2").arg(TmuxExec, name());

    int ret = system(qPrintable(cmd));
    if (!ret)
        m_open = true;

    return !ret;
}

bool TmuxSessionWrapper::redirectOutput(const QString& dest)
{
    if (!m_open)
        return false;

    QString cmd = QString("%1 pipe-pane -t %2 '/usr/bin/cat > %3'").arg(TmuxExec, name(), dest);

    int ret = system(qPrintable(cmd));
    return !ret;
}

bool TmuxSessionWrapper::sendKeys(const QString& keys)
{
    if (!m_open)
        return false;

    QString cmd = QString("%1 send-keys -t %2 '%3' C-m").arg(TmuxExec, name(), keys);

    int ret = system(qPrintable(cmd));
    return !ret;
}

bool TmuxSessionWrapper::kill()
{
    if (!m_open)
        return false;

    QString cmd = QString("%1 kill-session -t %2").arg(TmuxExec, name());

    int ret = system(qPrintable(cmd));
    if (ret)
        m_open = false;

    return !ret;
}

void TmuxSessionWrapper::generateRandomName()
{
    m_name = []() {
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
