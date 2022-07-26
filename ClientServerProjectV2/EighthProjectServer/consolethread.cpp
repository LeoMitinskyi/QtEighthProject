#include "consolethread.h"
#include <iostream>
#include <QTextStream>

using namespace std;

ConsoleThread::ConsoleThread() {}

void ConsoleThread::run()
{
    string command;
    cout << "To start server enter \'start\' and \'stop\' to stop server: " ;
    cout.flush();
    for (;;) {
        cin >> command;
        if (command == "stop") {
            cout << "You have entered \'stop\' command!" << endl;
            cout << "-> ";
            emit signalStopServer();
        } else if (command == "start") {
            cout << "You have entered \'start\' command!" << endl;
            cout << "-> ";
            emit signalStartServer();
        } else {
            cout << "Unknown command!" << endl;
            cout << "-> ";
        }
    }
}
