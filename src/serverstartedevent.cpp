#include "serverstartedevent.h"

namespace Morgoth {

ServerStartedEvent::ServerStartedEvent(QObject *parent) :
    EventHandler(Name, parent)
{

}

QRegularExpression ServerStartedEvent::regex() const
{
    return QRegularExpression("^(\\w+.so)\\sloaded for \"(.[^\"]+)\"$");
}

void ServerStartedEvent::maybeActivated(const QString& line, const QRegularExpressionMatch& match)
{
    Q_UNUSED(line);
    m_game = match.captured(2);
    emit activated();
}

} // namespace Morgoth
