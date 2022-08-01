#include "console.h"
#include <iostream>
#include <QDebug>

Console::Console() : QObject() {}

void Console::write(std::string s)
{
    std::cout << s;
    std::cout.flush();
}

void Console::writeLine(std::string s)
{
    write(s + "\n");
}

std::string Console::read()
{
    std::string s;
    std::cin >> s;
    return s;
}

void Console::run()
{
    qDebug() << "flag";
    while (m_running)
    {
        std::string command;
        write("To start server enter \'start\' and \'stop\' to stop server: ");
        for (;;) {
            command = read();
            if (command == "stop") {
                writeLine("You have entered \'stop\' command!");
                write("-> ");
                emit signalStopServer();
            } else if (command == "start") {
                writeLine("You have entered \'start\' command!");
                write("-> ");
                emit signalStartServer();
            } else {
                writeLine("Unknown command!");
                write("-> ");
            }
        }
    }
    emit finished();
}

void Console::setRunning(bool running)
{
    if (m_running == running)
        return;

    m_running = running;
    emit runningChanged(running);
}
