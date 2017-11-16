#ifndef LOGLISTENER_H
#define LOGLISTENER_H

#include "eventhandler.h"
#include <QtCore/QList>
#include <QtCore/QMutex>
#include <QtCore/QRegularExpression>
#include <QtCore/QThread>
#include <functional>

namespace morgoth {

class LogListener : public QThread {
    Q_OBJECT

public:
    LogListener(const QString& filePath, QObject* parent = nullptr);

    void installEventHandler(EventHandler* handler);

    const QString& filePath() const { return m_filePath; }

protected:
    void run() override;

private:
    QString m_filePath;
    bool m_isFinished = false;
    QList<EventHandler*> m_events;
    QMutex m_eventListMutex;

};

} // namespace Morgoth

#endif // LOGLISTENER_H
