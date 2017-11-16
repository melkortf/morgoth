#ifndef STDINREADER_H
#define STDINREADER_H

#include <QtCore/QObject>

class QSocketNotifier;

namespace morgoth {

class StdinReader : public QObject {
    Q_OBJECT

signals:
    void lineReceived(const QString& line);

public:
    explicit StdinReader(QObject *parent = nullptr);

private slots:
    void readStdin();

private:
    QSocketNotifier* m_stdinNotifier;

};

} // namespace Morgoth

#endif // STDINREADER_H
