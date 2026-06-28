#include "addshipmentform.h"
#include "ui_addshipmentform.h"

#include <QCompleter>
#include <QMessageBox>

#include "../BusinessLogic/ProductManager/productmanager.h"
#include "../MainWindow/mainwindow.h"

AddShipmentForm::AddShipmentForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AddShipmentForm)
{
    ui->setupUi(this);
    connect(MainWindow::getBasicForm(),&BasicForm::switchToAddShipmentForm,
            this, &AddShipmentForm::getData);
}

AddShipmentForm::~AddShipmentForm()
{
    delete ui;
}

void AddShipmentForm::getData()
{
    QStringList data = ProductManager::getAllProducts();
    qDebug() << "getData";

    if (!ui || !ui->comboBox_article) {
        qCritical() << "UI or comboBox_article is not initialized.";
        return;
    }

    QComboBox *articleComboBox = ui->comboBox_article;

    if (ui->lineEdit_shipment) {
        ui->lineEdit_shipment->clear();
    }
    if (ui->spinBox_quantity) {
        ui->spinBox_quantity->setValue(1);
    }
    if (ui->dateEdit_date) {
        ui->dateEdit_date->setDate(QDate::currentDate());
    }

    articleComboBox->clear();

    if (data.isEmpty()) {
        return;
    }

    articleComboBox->setEditable(true);
    articleComboBox->setInsertPolicy(QComboBox::NoInsert);

    articleComboBox->setStyleSheet(
        "QComboBox {"
        "padding-right: 2px;"
        "}"
        "QComboBox QLineEdit {"
        "padding-right: 30px;"
        "}"
        );

    articleComboBox->setCompleter(new QCompleter(articleComboBox));
    articleComboBox->completer()->setFilterMode(Qt::MatchContains);
    articleComboBox->completer()->setCompletionMode(QCompleter::PopupCompletion);

    QSet<QString> uniqueArticles;

    for (const QString& productStr : data) {

        QStringList parts = productStr.split(" | ", Qt::SkipEmptyParts);

        if (parts.isEmpty()) {
            continue;
        }

        QString fullId = parts.value(0).trimmed();

        QString article;
        if (fullId.startsWith("ID: ", Qt::CaseInsensitive)) {
            article = fullId.mid(4).trimmed();
        } else {
            article = fullId;
        }

        if (!article.isEmpty()) {

            if (!uniqueArticles.contains(article)) {
                articleComboBox->addItem(article);
                uniqueArticles.insert(article);
            }
        }
    }

    if (articleComboBox->count() > 0) {
        articleComboBox->setCurrentIndex(0);
    }
}

void AddShipmentForm::on_pushButton_back_clicked()
{
    emit cancelAddShipment();
}


void AddShipmentForm::on_pushButton_conf_clicked()
{
    QString article = ui->comboBox_article->currentText().trimmed();
    if (article.isEmpty()) {
        QMessageBox::critical(this, "Ошибка ввода", "Необходимо выбрать или ввести артикул продукта.");
        ui->comboBox_article->setFocus();
        return;
    }

    QString recipient = ui->lineEdit_shipment->text().trimmed();
    if (recipient.isEmpty()) {
        QMessageBox::critical(this, "Ошибка ввода", "Поле 'Получатель' не может быть пустым.");
        ui->lineEdit_shipment->setFocus();
        return;
    }

    int quantity = ui->spinBox_quantity->value();
    if (quantity <= 0) {
        QMessageBox::critical(this, "Ошибка ввода", "Количество должно быть положительным числом.");
        ui->spinBox_quantity->setFocus();
        return;
    }

    QDate shipmentDate = ui->dateEdit_date->date();

    ProductManager::addShipment(article.toLongLong(), quantity, shipmentDate, recipient);

    emit cancelAddShipment();


}
