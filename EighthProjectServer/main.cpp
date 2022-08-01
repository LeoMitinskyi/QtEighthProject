#include <QCoreApplication>
#include "serverloop.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    ServerLoop serverLoop;

    return a.exec();
}
