#ifndef SERVERLOOP_H
#define SERVERLOOP_H

#include <QThread>
#include "console.h"
#include "server.h"

class ServerLoop : public QObject
{
    Q_OBJECT

public:
    ServerLoop();

private slots:
    void startFirstThread();
    void startSecondThread();
    void stopFirstThread();
    void stopSecondThread();

private:
    QThread thread_1;
    QThread thread_2;
    Console consoleObject;
    Server serverObject;
};

#endif // SERVERLOOP_H
