#include "addform.h"
#include "ui_addform.h"
#include "../BusinessLogic/ProductManager/productmanager.h"

#include <QKeyEvent>
#include <QDate>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QToolTip>

AddForm::AddForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AddForm)
{
    ui->setupUi(this);
    ui->dateEdit_dateIn->setDate(QDate::currentDate());
    ui->dateEdit_dateOut->setDate(QDate::currentDate());
}

AddForm::~AddForm()
{
    delete ui;
}

void AddForm::keyPressEvent(QKeyEvent *event){
    if(event->key() == Qt::Key_Escape || event->key() == Qt::MouseButton(4))
    {
        on_pushButton_Cancel_clicked();
    }
}

void AddForm::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::XButton1){
        on_pushButton_Cancel_clicked();
    }
}

void AddForm:: on_pushButton_Cancel_clicked()
{
    ui->lineEdit_name->clear();
    ui->lineEdit_supplier->clear();
    ui->lineEdit_recipient->clear();
    ui->textEdit_about->clear();
    ui->doubleSpinBox_price->clear();
    ui->spinBox_count->clear();
    ui->dateEdit_dateIn->setDate(QDate::currentDate());
    ui->dateEdit_dateOut->setDate(QDate::currentDate());

    ui->lineEdit_name->setStyleSheet("");
    ui->lineEdit_supplier->setStyleSheet("");
    ui->lineEdit_recipient->setStyleSheet("");
    ui->textEdit_about->setStyleSheet("");
    ui->doubleSpinBox_price->setStyleSheet("");
    ui->spinBox_count->setStyleSheet("");
    ui->spinBox_count->setStyleSheet("");

    emit Cancel();
}


void AddForm::on_pushButton_Add_clicked(){
    QString name = ui->lineEdit_name->text();
    QString supplier = ui->lineEdit_supplier->text();
    QString recipient = ui->lineEdit_recipient->text();
    QString about = ui->textEdit_about->toPlainText();
    double price = ui->doubleSpinBox_price->value();
    int count = ui->spinBox_count->value();
    QDate dateIn = ui->dateEdit_dateIn->date();
    QDate dateOut = ui->dateEdit_dateOut->date();

    bool hasErrors = false;

    if(name.trimmed().isEmpty()){
        ui->lineEdit_name->setStyleSheet("border:1px solid red; border-radius:4px;");
        hasErrors = true;
    }else{
        ui->lineEdit_name->setStyleSheet("");
    }

    if(supplier.trimmed().isEmpty()){
        ui->lineEdit_supplier->setStyleSheet("border:1px solid red; border-radius:4px;");
        hasErrors = true;
    }else{
        ui->lineEdit_supplier->setStyleSheet("");
    }

    if(recipient.trimmed().isEmpty()){
        ui->lineEdit_recipient->setStyleSheet("border:1px solid red; border-radius:4px;");
        hasErrors = true;
    }else{
        ui->lineEdit_recipient->setStyleSheet("");
    }

    if(about.trimmed().isEmpty()){
        ui->textEdit_about->setStyleSheet("border:1px solid red; border-radius:4px;");
        hasErrors = true;
    }else{
        ui->textEdit_about->setStyleSheet("");
    }

    if (count <= 0) {
        ui->spinBox_count->setToolTip("Количество должно быть больше нуля!");

        QToolTip::showText(ui->spinBox_count->mapToGlobal(QPoint(0, ui->spinBox_count->height())),
                           ui->spinBox_count->toolTip(),
                           ui->spinBox_count);

        hasErrors = true;
    } else {
        ui->spinBox_count->setStyleSheet("");
        ui->spinBox_count->setToolTip("");
    }

    if (price <= 0.0) {
        ui->doubleSpinBox_price->setToolTip("Цена должна быть больше нуля!");

        QToolTip::showText(ui->doubleSpinBox_price->mapToGlobal(QPoint(0, ui->doubleSpinBox_price->height())),
                           ui->doubleSpinBox_price->toolTip(),
                           ui->doubleSpinBox_price);
        hasErrors = true;
    } else {
        ui->doubleSpinBox_price->setStyleSheet("");
        ui->doubleSpinBox_price->setToolTip("");
    }

    if (hasErrors) {
        return;
    }

    qDebug() << "yesss!!!";

    if(ProductManager::insertProduct(name,count,price,dateIn,supplier,dateOut,recipient,about)){
        ui->lineEdit_name->clear();
        ui->lineEdit_supplier->clear();
        ui->lineEdit_recipient->clear();
        ui->textEdit_about->clear();
        ui->doubleSpinBox_price->clear();
        ui->spinBox_count->clear();
        ui->dateEdit_dateIn->setDate(QDate::currentDate());
        ui->dateEdit_dateOut->setDate(QDate::currentDate());

        ui->lineEdit_name->setStyleSheet("");
        ui->lineEdit_supplier->setStyleSheet("");
        ui->lineEdit_recipient->setStyleSheet("");
        ui->textEdit_about->setStyleSheet("");
        ui->doubleSpinBox_price->setStyleSheet("");
        ui->spinBox_count->setStyleSheet("");
        ui->spinBox_count->setStyleSheet("");
    }
}
