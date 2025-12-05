#include "loginform.h"
#include "ui_loginform.h"
#include "../BusinessLogic/authentication.h"

LoginForm::LoginForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LoginForm)
{
    ui->setupUi(this);
}

LoginForm::~LoginForm()
{
    delete ui;
}

void LoginForm::on_pushButton_auth_clicked()
{
    ui->label_error->clear();
    QString login = ui->lineEdit_login->text();
    QString password = ui->lineEdit_password->text();

    if (login.isEmpty()) {
        ui->lineEdit_login->setStyleSheet("border: 1px solid red; border-radius: 4px;");
        return;
    }

    else if(password.isEmpty()){
         ui->lineEdit_password->setStyleSheet("border: 1px solid red; border-radius: 4px;");
        return;
    }

    else {
        ui->lineEdit_login->setStyleSheet("border: 1px solid #696666; border-radius: 4px;");
        ui->lineEdit_password->setStyleSheet("border: 1px solid #696666; border-radius: 4px;");
    }

    if (Authentication::authentication(login, password))
    {
        ui->label_error->clear();
        ui->lineEdit_password->clear();
    }
    else
    {
        ui->label_error->setText("Неверный логин или пароль!");
        ui->lineEdit_password->clear();
    }
}
