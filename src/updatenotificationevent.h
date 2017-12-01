#ifndef UPDATENOTIFICATIONEVENT_H
#define UPDATENOTIFICATIONEVENT_H

#include "morgoth_export.h"
#include "eventhandler.h"

namespace morgoth {

class MORGOTH_EXPORT UpdateNotificationEvent : public EventHandler {
    Q_OBJECT

public:
    explicit UpdateNotificationEvent(QObject *parent = nullptr);

    QRegularExpression regex() const override;

    static auto constexpr Name = "updatenotification";

};

} // namespace morgoth

#endif // UPDATENOTIFICATIONEVENT_H
