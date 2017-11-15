#ifndef LOGLISTENER_H
#define LOGLISTENER_H

#include <QtCore/QList>
#include <QtCore/QMutex>
#include <QtCore/QRegularExpression>
#include <QtCore/QThread>
#include <functional>

namespace Morgoth {

class LogListener : public QThread {
    Q_OBJECT

public:
    LogListener(const QString& filePath, QObject* parent = nullptr);

    /**
     * \brief Adds event handler.
     * \note This function is thread-safe.
     * \param regex
     * \param handler
     */
    void addEvent(const QRegularExpression& regex, std::function<void(const QString&, const QRegularExpressionMatch&)> handler);

    const QString& filePath() const { return m_filePath; }

protected:
    void run() override;

private:
    using EventHandler = std::function<void(const QString&, const QRegularExpressionMatch&)>;
    using Event = QPair<QRegularExpression, EventHandler>;

    QString m_filePath;
    bool m_isFinished = false;
    QList<Event> m_events;
    QMutex m_eventListMutex;

};

} // namespace Morgoth

#endif // LOGLISTENER_H
