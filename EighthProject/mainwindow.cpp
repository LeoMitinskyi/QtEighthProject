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
        m_socket->write("{\"Type\":\"auth\", \"Login\":\"" + ba1 + "\", \"Password\":\"" + ba2 + "\"}");
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

        m_socket->write("{\"Type\":\"registration\", \"Name\":\"" + ba1 + "\", \"Surname\":\"" + ba2 + "\", \"Login\":\"" + ba3 + "\", \"Password\":\"" + ba4 + "\"}");
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

        qDebug() << (m_doc.object().value("Type").toString());
        qDebug() << (m_doc.object().value("Status").toString());
        if (m_docError.errorString() == "no error occurred")
        {
            if ((m_doc.object().value("Type").toString() == "connect") && (m_doc.object().value("Status").toString() == "yes"))
            {
                qDebug() << "Connected";
            } else if (m_doc.object().value("Type").toString() == "resultSelect") {
                qDebug() << "tableflag";
                QJsonArray docAr = m_doc.object().value("Result").toArray();
                emit signalQJsonArrayGetter(docAr);
            } else if ((m_doc.object().value("Type").toString() == "auth") && (m_doc.object().value("Status").toString() == "found")) {
                qDebug() << "flag";
                QMessageBox::information(this, "Information", "Success");
                hide();
                window->setModal(true);
                window->show();
                emit signalNameGetter(m_doc.object().value("Name").toString(), m_doc.object().value("Surname").toString());
                emit signalSocketGetter(m_socket);
                if (m_socket->isOpen())
                {
                    m_socket->write("{\"Type\":\"select\", \"Params\":\"ListOfProducts\"}");
                    m_socket->waitForBytesWritten(500);
                } else {
                    QMessageBox::information(this, "Information", "Not connected");
                }
            } else if ((m_doc.object().value("Type").toString() == "auth") && (m_doc.object().value("Status").toString() == "notfound")) {
                QMessageBox::warning(this, "Notification", "Not found!");
            } else if ((m_doc.object().value("Type").toString() == "registration") && (m_doc.object().value("Status").toString() == "added")) {
                QMessageBox::information(this, "Notification", "User was added!");
            } else if ((m_doc.object().value("Type").toString() == "registration") && (m_doc.object().value("Status").toString() == "taken")) {
                QMessageBox::warning(this, "Notification", "This login is taken!");
            } else {
                QMessageBox::information(this, "Information", "Not connected");
            }
        } else {
            QMessageBox::information(this, "Information", "Format reading error" + m_docError.errorString());
        }
    }
}
