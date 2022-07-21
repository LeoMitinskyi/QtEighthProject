#ifndef SECONDWINDOW_H
#define SECONDWINDOW_H

#include <QMainWindow>
#include <QDialog>
#include <QtWidgets>
#include <QTcpSocket>
#include <QSqlTableModel>
#include <QKeyEvent>

namespace Ui {
class SecondWindow;
}

class SecondWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SecondWindow(QWidget *parent = nullptr);
    ~SecondWindow();

private:
    void loadTable();

private slots:
    void on_QuitButton_clicked();
    void showTime();
    void on_tableView_clicked(const QModelIndex &index);
    void on_deleteButton_clicked();
    void on_addButton_clicked();
    void on_pushButton_clicked();

private:
    Ui::SecondWindow *ui;
    QStandardItemModel* m_model;
    QTcpSocket *m_socket;
    int m_row;

public slots:
    void slotNameGetter(const QString &name, const QString &sirname);
    void slotlQJsonArrayGetter(QJsonArray&);
    void slotSocketGetter(QTcpSocket *socket);
};

#endif // SECONDWINDOW_H
