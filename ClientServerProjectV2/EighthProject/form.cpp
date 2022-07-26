#include "form.h"
#include "ui_form.h"

Form::Form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form)
{
    ui->setupUi(this);
}

Form::~Form()
{
    delete ui;
}

void Form::on_pushButton_clicked()
{
    auto name = ui->nameEdit->text();
    auto sirname = ui->sirnameEdit->text();
    auto login = ui->loginEdit->text();
    auto password = ui->passwordEdit->text();

    emit signalRegistrationDataGetter(name, sirname, login, password);
    hide();
}

