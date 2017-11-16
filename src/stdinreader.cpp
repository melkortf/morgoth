#include "stdinreader.h"
#include <QtCore>
#include <unistd.h>

namespace morgoth {

StdinReader::StdinReader(QObject *parent) :
    QObject(parent),
    m_stdinNotifier(new QSocketNotifier(STDIN_FILENO, QSocketNotifier::Read))
{
    connect(m_stdinNotifier, &QSocketNotifier::activated, this, &StdinReader::readStdin);
}

void StdinReader::readStdin()
{
    QTextStream tsin(stdin);
    QString line = tsin.readLine();
    emit lineReceived(line);
}

} // namespace Morgoth
