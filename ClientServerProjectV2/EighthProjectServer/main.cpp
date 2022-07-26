#include <QCoreApplication>
#include "server.h"
#include "consolethread.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Server server;

    return a.exec();
}
