#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "form.h"
#include "secondwindow.h"
#include <QTcpSocket>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QStandardItem>
#include <QJsonArray>
#include <QStandardItemModel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_authButton_clicked();
    void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;
    SecondWindow *window;
    Form *form;
    QTcpSocket *m_socket;
    QByteArray m_Data;
    QJsonDocument m_doc;
    QJsonParseError m_docError;

public slots:
    void slotRegistrationDataGetter(const QString &name, const QString &sirname, const QString &login, const QString &password);
    void sockReady();
    void sockDisc();

signals:
    void signalNameGetter(const QString&, const QString&);
    void signalQJsonArrayGetter(QJsonArray&);
    void signalSocketGetter(QTcpSocket*);
};
#endif // MAINWINDOW_H
