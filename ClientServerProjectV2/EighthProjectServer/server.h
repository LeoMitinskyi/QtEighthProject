#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QVector>
#include <QDateTime>
#include <QSqlDatabase>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QSqlError>
#include <QCryptographicHash>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

class Server : public QTcpServer
{
    Q_OBJECT

public:
    Server();
    ~Server();

private:
    QTcpSocket* m_socket;
    QByteArray m_Data;
    QJsonDocument m_doc;
    QJsonParseError m_docError;
    QSqlDatabase m_db;

private:
    void checkLoginAndPassword(const QString&, const QString&);
    void addNewUser(const QString&, const QString&, const QString&, const QString&);
    void addNewItem(const QString&, const QString&);
    void deleteItem(const QString&, const QString&);

public slots:
    void startServer();
    void incomingConnection(qintptr socketDescription);
    void sockReady();
    void sockDisc();
    void slotStopServer();

signals:
    void signalSendInfo(const QString&);
};

#endif // SERVER_H
