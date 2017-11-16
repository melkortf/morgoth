#ifndef SERVERSTARTEDEVENT_H
#define SERVERSTARTEDEVENT_H

#include "eventhandler.h"

namespace morgoth {

class ServerStartedEvent : public EventHandler {
    Q_OBJECT

public:
    explicit ServerStartedEvent(QObject *parent = nullptr);

    QRegularExpression regex() const override;

    const QString& game() const { return m_game; }

    static auto constexpr Name = "server.start";

protected:
    void maybeActivated(const QString& line, const QRegularExpressionMatch& match) override;

private:
    QString m_game;

};

} // namespace Morgoth

#endif // SERVERSTARTEDEVENT_H
