#include "server.h"

Server::Server(){}

Server::~Server(){}

void Server::checkLoginAndPassword(const QString &login, const QString &password)
{
    QByteArray bytes = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Md5);
    QString digest = QString(bytes.toHex());

    QSqlQuery* query = new QSqlQuery(m_db);
    query->prepare("SELECT Firstname, Lastname From Users Where Login == :login And Password == :password;");
    query->bindValue(":login", login);
    query->bindValue(":password", digest);

    qDebug() << digest;

    if (query->exec()) {
        query->next();
        if (!query->value(0).isNull()){
        const QString name = query->value(0).toString();
        const QString sirname = query->value(1).toString();
        qDebug() << name << sirname;
        QByteArray ba1 = name.toLocal8Bit();
        QByteArray ba2 = sirname.toLocal8Bit();
        m_socket->write("{\"type\":\"auth\",\"status\":\"found\",\"name\":\"" + ba1 + "\",\"sirname\":\"" + ba2 + "\"}");
        } else {
            qDebug() << "Not found";
            m_socket->write("{\"type\":\"auth\",\"status\":\"notfound\"}");
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
            qDebug() << "Added";
            m_socket->write("{\"type\":\"registration\",\"status\":\"added\"}");
        } else {
            qDebug() << "Taken";
            m_socket->write("{\"type\":\"registration\",\"status\":\"taken\"}");
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

void Server::startServer()
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

    m_socket->write("{\"type\":\"connect\",\"status\":\"yes\"}");
    qDebug() << "Send to client connect status - YES";
}

void Server::sockDisc()
{
    qDebug() << "Disconnect";
    m_socket->deleteLater();
}

void Server::sockReady()
{
    m_Data = m_socket->readAll();
    qDebug() << "Data: " << m_Data;

    m_doc = QJsonDocument::fromJson(m_Data, &m_docError);

    if (m_docError.errorString()=="no error occurred")
    {
        if ((m_doc.object().value("type").toString() == "select") && (m_doc.object().value("params").toString() == "ListOfProducts"))
        {
            if (m_db.isOpen())
            {
                QByteArray result = "{\"type\":\"resultSelect\",\"result\":[";
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
        } else if (m_doc.object().value("type").toString() == "auth") {
            if (m_db.open()){
                auto login = m_doc.object().value("login").toString();
                auto password = m_doc.object().value("login").toString();
                checkLoginAndPassword(login, password);
            }
        } else if (m_doc.object().value("type").toString() == "registration") {
            if (m_db.open()){
                auto name = m_doc.object().value("name").toString();
                auto sirname =  m_doc.object().value("sirname").toString();
                auto login = m_doc.object().value("login").toString();
                auto password =  m_doc.object().value("password").toString();
                addNewUser(name, sirname, login, password);
            }
        } else if (m_doc.object().value("type").toString() == "insertItem") {
            if (m_db.open()){
                auto name = m_doc.object().value("name").toString();
                auto weight = m_doc.object().value("weight").toString();
                addNewItem(name, weight);
            }
        } else if (m_doc.object().value("type").toString() == "deleteItem") {
            if (m_db.open()){
                auto name =  m_doc.object().value("name").toString();
                auto weight = m_doc.object().value("weight").toString();
                deleteItem(name, weight);
            }
        }
    }
}
