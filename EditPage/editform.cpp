#include "editform.h"
#include "ui_editform.h"

#include <QMessageBox>

#include "../MainWindow/mainwindow.h"
#include "../BusinessLogic/ProductManager/productmanager.h"

qint64 EditForm::id = -1;

EditForm::EditForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::EditForm)
{
    ui->setupUi(this);
    connect(MainWindow::getBasicForm(),&BasicForm::switchToEditForm,
            this, &EditForm::updateData);
}

EditForm::~EditForm()
{
    delete ui;
}

void EditForm::updateData(qint64 id){
    qDebug() << "Загрузка данных для продукта ID:" << id;
    QStringList data = ProductManager::getProductDataById(id);


    if (data.size() < 5) {
        qWarning() << "Ошибка: Получено недостаточно данных для заполнения формы. Size:" << data.size();
        return;
    }

    // Порядок полей из getProductDataById: 0=id, 1=name, 2=about, 3=price, 4=quantity
    ui->lineEdit_name->setText(data[1]);
    ui->textEdit_about->setText(data[2]);
    ui->doubleSpinBox_price->setValue(data[3].toDouble());
    ui->spinBox_count->setValue(data[4].toInt());

    for(int i = 1; i < 5; i++){
        qDebug() << "data[" << i << "]:" << data[i];
    }

    EditForm::id = id;
}

void EditForm::on_pushButton_back_clicked()
{
    emit Cancel();
}

void EditForm::on_pushButton_conf_clicked()
{
    qDebug() << "conf";
    QString name = ui->lineEdit_name->text();
    QString about = ui->textEdit_about->toPlainText();
    double price = ui->doubleSpinBox_price->value();
    int count = ui->spinBox_count->value();
    if(!name.trimmed().isEmpty() && !about.trimmed().isEmpty() && price > 0.0 && count > 0){
        ProductManager::updateProduct(EditForm::id, name,price,about);
    }else{
        QMessageBox::information(
            nullptr,
            "Ошибка",
            "Заполните все поля!!"
            );
        return;
    }
    emit Cancel();
}
