#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "secondwindow.h"
#include <QtWidgets>
#include <QSqlError>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    form = new Form;
    window = new SecondWindow();

    m_socket = new QTcpSocket(this);
    connect(m_socket, SIGNAL(readyRead()), this, SLOT(sockReady()));
    connect(m_socket, SIGNAL(disconnected()), this, SLOT(sockDisc()));

    m_socket->connectToHost("127.0.0.1", 2323);

    connect(form, &Form::signalRegistrationDataGetter, this, &MainWindow::slotRegistrationDataGetter);
    connect(this, &MainWindow::signalNameGetter, window, &SecondWindow::slotNameGetter);
    connect(this, &MainWindow::signalQJsonArrayGetter, window, &SecondWindow::slotlQJsonArrayGetter);
    connect(this, &MainWindow::signalSocketGetter, window, &SecondWindow::slotSocketGetter);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::sockDisc()
{
    m_socket->deleteLater();
}

void MainWindow::on_authButton_clicked()
{
    if (m_socket->isOpen())
    {
        QString login = ui->loginEdit->text();
        QString password = ui->passwordEdit->text();
        QByteArray ba1 = login.toLocal8Bit();
        QByteArray ba2 = password.toLocal8Bit();
        m_socket->write("{\"type\":\"auth\", \"login\":\"" + ba1 + "\", \"password\":\"" + ba2 + "\"}");
    } else {
        QMessageBox::information(this, "Information", "Not connected");
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    form->show();
}

void MainWindow::slotRegistrationDataGetter(const QString &name, const QString &sirname, const QString &login, const QString &password)
{
    if (m_socket->isOpen())
    {
        QByteArray ba1 = name.toLocal8Bit();
        QByteArray ba2 = sirname.toLocal8Bit();
        QByteArray ba3 = login.toLocal8Bit();
        QByteArray ba4 = password.toLocal8Bit();

        m_socket->write("{\"type\":\"registration\", \"name\":\"" + ba1 + "\", \"sirname\":\"" + ba2 + "\", \"login\":\"" + ba3 + "\", \"password\":\"" + ba4 + "\"}");
        m_socket->waitForBytesWritten(500);

    } else {
        QMessageBox::information(this, "Information", "Not connected");
    }
}

void MainWindow::sockReady()
{
    if (m_socket->waitForConnected(500))
    {
        m_socket->waitForReadyRead(500);
        m_Data = m_socket->readAll();
        m_doc = QJsonDocument::fromJson(m_Data, &m_docError);

        if (m_docError.errorString() == "no error occurred")
        {
            if ((m_doc.object().value("type").toString() == "connect") && (m_doc.object().value("status").toString() == "yes"))
            {
                qDebug() << "Connected";
            } else if (m_doc.object().value("type").toString() == "resultSelect") {
                QJsonArray docAr = m_doc.object().value("result").toArray();
                emit signalQJsonArrayGetter(docAr);
            } else if ((m_doc.object().value("type").toString() == "auth") && (m_doc.object().value("status").toString() == "found")) {
                QMessageBox::information(this, "Information", "Success");
                hide();
                window->setModal(true);
                window->show();
                emit signalNameGetter(m_doc.object().value("name").toString(), m_doc.object().value("sirname").toString());
                emit signalSocketGetter(m_socket);
                if (m_socket->isOpen())
                {
                    m_socket->write("{\"type\":\"select\", \"params\":\"ListOfProducts\"}");
                    m_socket->waitForBytesWritten(500);
                } else {
                    QMessageBox::information(this, "Information", "Not connected");
                }
            } else if ((m_doc.object().value("type").toString() == "auth") && (m_doc.object().value("status").toString() == "notfound")) {
                QMessageBox::warning(this, "Notification", "Not found!");
            } else if ((m_doc.object().value("type").toString() == "registration") && (m_doc.object().value("status").toString() == "added")) {
                QMessageBox::information(this, "Notification", "User was added!");
            } else if ((m_doc.object().value("type").toString() == "registration") && (m_doc.object().value("status").toString() == "taken")) {
                QMessageBox::warning(this, "Notification", "This login is taken!");
            } else {
                QMessageBox::information(this, "Information", "Not connected");
            }
        } else {
            QMessageBox::information(this, "Information", "Format reading error" + m_docError.errorString());
        }
    }
}
