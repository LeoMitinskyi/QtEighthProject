#include "server.h"
#include "console.h"
#include <QTcpSocket>
#include "console.h"
#include <QApplication>

Server::Server() : QTcpServer() {}

void Server::checkLoginAndPassword(const QString &login, const QString &password)
{
    QByteArray bytes = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Md5);
    QString digest = QString(bytes.toHex());

    QSqlQuery* query = new QSqlQuery(m_db);
    query->prepare("SELECT Firstname, Lastname From Users Where Login == :login And Password == :password;");
    query->bindValue(":login", login);
    query->bindValue(":password", digest);

    if (query->exec()) {
        query->next();
        if (!query->value(0).isNull()){
        m_socket->write("{\"Type\":\"auth\",\"Status\":\"found\",\"Name\":\"" + query->value(0).toString().toLocal8Bit() + "\",\"Surname\":\"" + query->value(1).toString().toLocal8Bit() + "\"}");
        } else {
            m_socket->write("{\"Type\":\"auth\",\"Status\":\"notfound\"}");
        }
    } else {
        qDebug() << "db error";
    }
}

void Server::addNewUser(const QString &name, const QString &sirname, const QString &login, const QString &password)
{
    QSqlQuery* query = new QSqlQuery(m_db);
    query->prepare("SELECT Firstname FROM Users Where Login == :login;");
    query->bindValue(":login", login);

    if (query->exec()){
        query->next();
        if (query->value(0).isNull()) {
            QByteArray bytes = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Md5);
            QString digest = QString(bytes.toHex());
            query->prepare("INSERT INTO Users (Firstname, Lastname, Login, Password) VALUES (:firstname, :lastname, :login, :password);");
            query->bindValue(":firstname", name);
            query->bindValue(":lastname", sirname);
            query->bindValue(":login", login);
            query->bindValue(":password", digest);
            query->exec();
            m_socket->write("{\"Type\":\"registration\",\"Status\":\"added\"}");
        } else {
            m_socket->write("{\"Type\":\"registration\",\"Status\":\"taken\"}");
        }
    }
}

void Server::addNewItem(const QString &name, const QString &weight)
{
    QSqlQuery* query = new QSqlQuery(m_db);
    query->prepare("INSERT INTO ListOfProducts (Name, Weight) VALUES (:name, :weight);");
    query->bindValue(":name", name);
    query->bindValue(":weight", weight);
    if (query->exec()) {
        qDebug() << "New item was added";
    } else {
        qDebug() << "db error";
    }
}

void Server::deleteItem(const QString &name, const QString &weight)
{
    QSqlQuery* query = new QSqlQuery(m_db);
    query->prepare("DELETE FROM ListOfProducts Where Name = :name And Weight = :weight;");
    query->bindValue(":name", name);
    query->bindValue(":weight", weight);
    if (query->exec()) {
        qDebug() << "Item was deleted";
    } else {
        qDebug() << "db error";
    }
}

void Server::startServerLoop()
{
    if (this->listen(QHostAddress::Any, 2323)){
        m_db = QSqlDatabase::addDatabase("QSQLITE");
        m_db.setDatabaseName("usersDB.db");
        if (m_db.open()){
            qDebug() << "Listening and Open db";
        } else {
            qDebug() << "db not open";
        }
    } else {
        qDebug() << "Not listening";
    }
}

void Server::incomingConnection(qintptr socketDescriptor)
{
    m_socket = new QTcpSocket(this);
    m_socket->setSocketDescriptor(socketDescriptor);
    connect(m_socket, SIGNAL(readyRead()), this, SLOT(sockReady()));
    connect(m_socket, SIGNAL(disconnected()), this, SLOT(sockDisc()));

    qDebug() << "Client connected:  " << socketDescriptor;

    m_socket->write("{\"Type\":\"connect\",\"Status\":\"yes\"}");
    qDebug() << "Send to client connect status - YES";
}

void Server::sockDisc()
{
    m_socket->deleteLater();
}

void Server::slotStopServer()
{
    m_db.close();
    m_socket->close();
    close();
    exit(0);
}

void Server::run()
{
    startServerLoop();
}

void Server::setRunning(bool running)
{
    if (m_running == running)
        return;

    m_running = running;
    emit runningChanged(running);
}

void Server::sockReady()
{
    m_Data = m_socket->readAll();

    m_doc = QJsonDocument::fromJson(m_Data, &m_docError);

    if (m_docError.errorString()=="no error occurred")
    {
        if ((m_doc.object().value("Type").toString() == "select") && (m_doc.object().value("Params").toString() == "ListOfProducts"))
        {
            if (m_db.isOpen())
            {
                QByteArray result = "{\"Type\":\"resultSelect\",\"Result\":[";
                QSqlQuery* query = new QSqlQuery(m_db);
                if (query->exec("SELECT Name, Weight FROM ListOfProducts;"))
                {
                    while (query->next())
                    {
                        result.append("{\"Name\":\"" + query->value(0).toByteArray() + "\"},");
                        result.append("{\"Weight\":\"" + query->value(1).toByteArray() + "\"},");
                    }
                    result.remove(result.length() - 1, 1);
                    result.append("]}");

                    m_socket->write(result);
                    m_socket->waitForBytesWritten(500);
                } else {
                    qDebug() << "Query not success";
                }
            }
        } else if (m_doc.object().value("Type").toString() == "auth") {
            if (m_db.open()){
                auto login = m_doc.object().value("Login").toString();
                auto password = m_doc.object().value("Login").toString();
                checkLoginAndPassword(login, password);
            }
        } else if (m_doc.object().value("Type").toString() == "registration") {
            if (m_db.open()){
                auto name = m_doc.object().value("Name").toString();
                auto sirname =  m_doc.object().value("Surname").toString();
                auto login = m_doc.object().value("Login").toString();
                auto password =  m_doc.object().value("Password").toString();
                addNewUser(name, sirname, login, password);
            }
        } else if (m_doc.object().value("Type").toString() == "insertItem") {
            if (m_db.open()){
                auto name = m_doc.object().value("Name").toString();
                auto weight = m_doc.object().value("Weight").toString();
                addNewItem(name, weight);
            }
        } else if (m_doc.object().value("Type").toString() == "deleteItem") {
            if (m_db.open()){
                auto name =  m_doc.object().value("Name").toString();
                auto weight = m_doc.object().value("Weight").toString();
                deleteItem(name, weight);
            }
        }
    }
}
