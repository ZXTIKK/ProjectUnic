#include "addform.h"
#include "ui_addform.h"
#include "../BusinessLogic/ProductManager/productmanager.h"

#include <QKeyEvent>
#include <QDate>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QToolTip>
#include <QDebug>

AddForm::AddForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AddForm)
{
    ui->setupUi(this);
    ui->dateEdit_dateIn->setDate(QDate::currentDate());
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
    ui->textEdit_about->clear();
    ui->doubleSpinBox_price->clear();
    ui->spinBox_count->clear();
    ui->dateEdit_dateIn->setDate(QDate::currentDate());

    ui->lineEdit_name->setStyleSheet("");
    ui->lineEdit_supplier->setStyleSheet("");
    ui->textEdit_about->setStyleSheet("");
    ui->doubleSpinBox_price->setStyleSheet("");
    ui->spinBox_count->setStyleSheet("");
    ui->spinBox_count->setStyleSheet("");

    emit Cancel();
}


void AddForm::on_pushButton_Add_clicked(){
    QString name = ui->lineEdit_name->text();
    QString supplier = ui->lineEdit_supplier->text();

    QString about = ui->textEdit_about->toPlainText();
    double price = ui->doubleSpinBox_price->value();

    int supplies_quantity = ui->spinBox_count->value();
    QDate supplies_date = ui->dateEdit_dateIn->date();

    QDate created_at = QDate::currentDate();


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

    if(about.trimmed().isEmpty()){
        ui->textEdit_about->setStyleSheet("border:1px solid red; border-radius:4px;");
        hasErrors = true;
    }else{
        ui->textEdit_about->setStyleSheet("");
    }

    if (supplies_quantity <= 0) {
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

    qDebug() << "Attempting to insert product with initial supply...";

    if(ProductManager::insertProduct(
            name,
            price,
            supplier,
            about,
            supplies_quantity,
            supplies_date))
    {
        ui->lineEdit_name->clear();
        ui->lineEdit_supplier->clear();
        ui->textEdit_about->clear();
        ui->doubleSpinBox_price->clear();
        ui->spinBox_count->clear();
        ui->dateEdit_dateIn->setDate(QDate::currentDate());


        ui->lineEdit_name->setStyleSheet("");
        ui->lineEdit_supplier->setStyleSheet("");
        ui->textEdit_about->setStyleSheet("");
        ui->doubleSpinBox_price->setStyleSheet("");
        ui->spinBox_count->setStyleSheet("");
    } else {
        qCritical() << "Failed to insert product and initial supply.";
    }
}
