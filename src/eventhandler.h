#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include <QtCore/QObject>
#include <QtCore/QRegularExpression>

namespace morgoth {

class LogListener;

class EventHandler : public QObject {
    Q_OBJECT

signals:
    void activated();

public:
    explicit EventHandler(const QString& name, QObject* parent = nullptr);

    virtual QRegularExpression regex() const = 0;

    const QString& name() const { return m_name; }

protected:
    virtual void maybeActivated(const QString& line, const QRegularExpressionMatch& match);

private:
    friend class LogListener;
    QString m_name;

};

} // namespace Morgoth

#endif // EVENTHANDLER_H
