#include "serverloop.h"

ServerLoop::ServerLoop()
{
    connect(&thread_1, &QThread::started, &consoleObject, &Console::run);
    connect(&thread_2, &QThread::started, &serverObject, &Server::run);

    connect(&consoleObject, &Console::finished, &thread_1, &QThread::terminate);
    connect(&serverObject, &Server::finished, &thread_2, &QThread::terminate);

    // connect(&consoleObject, &Console::signalStartServer, &serverObject, &Server::startServerLoop, Qt::DirectConnection);
    connect(&consoleObject, &Console::signalStartServer, this, &ServerLoop::startSecondThread);
    connect(&consoleObject, &Console::signalStopServer, this, &ServerLoop::stopSecondThread, Qt::DirectConnection);

    consoleObject.moveToThread(&thread_1);
    serverObject.moveToThread(&thread_2);

    startFirstThread();
}

void ServerLoop::startFirstThread()
{
    consoleObject.setRunning(true);
    thread_1.start();
}

void ServerLoop::startSecondThread()
{
    serverObject.setRunning(true);
    thread_2.start();
}

void ServerLoop::stopFirstThread()
{
    consoleObject.setRunning(false);
}

void ServerLoop::stopSecondThread()
{
    serverObject.slotStopServer();
    serverObject.setRunning(false);
    stopFirstThread();
}
