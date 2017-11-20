#ifndef MORGOTHDAEMON_H
#define MORGOTHDAEMON_H

#include <QtCore/QObject>
#include <QtCore/QSocketNotifier>

namespace morgoth {

/**
 * @brief The MorgothDaemon class makes morgoth a proper posix daemon.
 */
class MorgothDaemon : public QObject {
    Q_OBJECT

public:
    MorgothDaemon(QObject* parent = nullptr);

private slots:
    void handleSignal();

private:
    QSocketNotifier* m_signal;

};

} // namespace morgoth

#endif // MORGOTHDAEMON_H
