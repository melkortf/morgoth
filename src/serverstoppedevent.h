#ifndef SERVERSTOPPEDEVENT_H
#define SERVERSTOPPEDEVENT_H

#include "eventhandler.h"

namespace morgoth {

class ServerStoppedEvent : public EventHandler {
    Q_OBJECT

public:
    explicit ServerStoppedEvent(QObject *parent = nullptr);

    QRegularExpression regex() const override;

    static auto constexpr Name = "server.stopped";

};

} // namespace Morgoth

#endif // SERVERSTOPPEDEVENT_H
