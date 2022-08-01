#ifndef CONSOLE_H
#define CONSOLE_H

#include <QThread>

class Console : public QObject
{
    Q_OBJECT

public:
    explicit Console();
    void write(std::string);
    void writeLine(std::string);
    std::string read();
    bool m_running;

public:signals:
    void signalStopServer();
    void signalStartServer();
    void finished();
    void runningChanged(bool running);

public slots:
    void run();
    void setRunning(bool running);
};

#endif // CONSOLE_H
