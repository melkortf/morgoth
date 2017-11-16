#include "eventhandler.h"

namespace morgoth {

EventHandler::EventHandler(const QString &name, QObject* parent) :
    QObject(parent),
    m_name(name) {}

void EventHandler::maybeActivated(const QString& line, const QRegularExpressionMatch& match)
{
    Q_UNUSED(line);
    Q_UNUSED(match);
    emit activated();
}

} // namespace Morgoth
