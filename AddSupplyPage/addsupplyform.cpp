#include "addsupplyform.h"
#include "ui_addsupplyform.h"

#include <QCompleter>
#include <QMessageBox>
#include <QDate>

#include "../MainWindow/mainwindow.h"
#include "../BusinessLogic/ProductManager/productmanager.h"

AddSupplyForm::AddSupplyForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AddSupplyForm)
{
    ui->setupUi(this);
    connect(MainWindow::getBasicForm(),&BasicForm::switchToAddSupplyForm,
            this, &AddSupplyForm::getData);
}

AddSupplyForm::~AddSupplyForm()
{
    delete ui;
}

void AddSupplyForm::on_pushButton_back_clicked()
{
    emit cancelAddSupply();
}


void AddSupplyForm::on_pushButton_conf_clicked()
{
    QString article = ui->comboBox_article->currentText().trimmed();
    if (article.isEmpty()) {
        QMessageBox::critical(this, "Ошибка ввода", "Необходимо выбрать или ввести артикул продукта.");
        ui->comboBox_article->setFocus();
        return;
    }

    QString supplier = ui->lineEdit_supplier->text().trimmed();
    if (supplier.isEmpty()) {
        QMessageBox::critical(this, "Ошибка ввода", "Поле 'Поставщик' не может быть пустым.");
        ui->lineEdit_supplier->setFocus();
        return;
    }

    int quantity = ui->spinBox_quantity->value();
    if (quantity <= 0) {
        QMessageBox::critical(this, "Ошибка ввода", "Количество должно быть положительным числом.");
        ui->spinBox_quantity->setFocus();
        return;
    }

    QDate supplyDate = ui->dateEdit_date->date();

    ProductManager::addSupply(article.toLongLong(), quantity, supplyDate, supplier);

    emit cancelAddSupply();


}

void AddSupplyForm::getData()
{
    QStringList data = ProductManager::getAllProducts();

    if (!ui || !ui->comboBox_article) {
        qCritical() << "UI or comboBox_article is not initialized.";
        return;
    }

    QComboBox *articleComboBox = ui->comboBox_article;

    if (ui->lineEdit_supplier) {
        ui->lineEdit_supplier->clear();
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
