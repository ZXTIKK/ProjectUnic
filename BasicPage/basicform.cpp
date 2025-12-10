#include "basicform.h"
#include "ui_basicform.h"

#include "../LoginPage/loginform.h"
#include "../MainWindow/mainwindow.h"
#include "../BusinessLogic/ProductManager/productmanager.h"

#include <QDebug>
#include <QLabel>
#include <QTableView>


BasicForm::BasicForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BasicForm)
{
    ui->setupUi(this);

    LoginForm *loginForm = MainWindow::getLoginForm();

    connect(loginForm, &LoginForm::loginSuccessful,
            this, &BasicForm::updateDate);
}

BasicForm::~BasicForm()
{
    delete ui;
}

void BasicForm::on_pushButton_add_clicked()
{
    emit PageAdd();
}

void BasicForm::updateDate()
{
    QStringList products = ProductManager::getAllProducts();
    if(products.length()<1){
        QLabel *labelNotFound = new QLabel(ui->scrollArea);
        labelNotFound->setText("Не найдены товары");
    }else{
        delete lableNotFound;
    }
    for(int i = 0; i < products.length(); i++){
        qDebug() << products[i];
    }

}
