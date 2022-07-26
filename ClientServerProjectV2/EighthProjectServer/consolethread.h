#ifndef CONSOLETHREAD_H
#define CONSOLETHREAD_H

#include <QThread>

class ConsoleThread : public QThread
{
    Q_OBJECT

public:
    explicit ConsoleThread();
    void run();

public:signals:
    void signalStopServer();
    void signalStartServer();
};

#endif // CONSOLETHREAD_H
