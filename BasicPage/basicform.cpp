#include "basicform.h"
#include "ui_basicform.h"

#include "../LoginPage/loginform.h"
#include "../MainWindow/mainwindow.h"
#include "../BusinessLogic/ProductManager/productmanager.h"

#include <QTableWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QWidget>
#include <QHeaderView>
#include <QDebug>
#include <QFont>
#include <QMessageBox>

BasicForm::BasicForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BasicForm)
{
    ui->setupUi(this);

    LoginForm *loginForm = MainWindow::getLoginForm();
    EditForm *editForm = MainWindow::getEditForm();
    AddForm *addForm = MainWindow::getAddForm();

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

void BasicForm::on_pushButton_about_clicked()
{
    emit switchToAbout();
}

void BasicForm::clearScrollArea()
{
    if (!m_tableProducts.isNull()) {
        m_tableProducts->deleteLater();
        m_tableProducts = nullptr;
    }
    if (!m_labelNotFound.isNull()) {
        m_labelNotFound->deleteLater();
        m_labelNotFound = nullptr;
    }
}

void BasicForm::updateDate()
{
    qDebug() << "update!";
    QStringList products = ProductManager::getAllProducts();
    addDataInTable(products);
}

void BasicForm::on_deleteButton_clicked(qint64 id)
{
    qDebug() << "Нажата кнопка 'удалить' для продукта с ID:" << id;
    if(ProductManager::deleteProduct(id)){
        updateDate();
    }else{
        QMessageBox::information(
            nullptr,
            "Ошибка",
            "Произошла ошибка!!"
            );
    }
}

void BasicForm::on_editButton_clicked(qint64 id)
{
    qDebug() << "Нажата кнопка 'изменить' для продукта с ID:" << id;
    emit switchToEditForm(id);
}

void BasicForm::on_pushButton_supply_clicked()
{
    emit switchToAddSupplyForm();

}

void BasicForm::on_pushButton_ShipmentForm_clicked()
{
    emit switchToAddShipmentForm();
}

void BasicForm::on_pushButton_find_clicked(){
    if(!ui->lineEdit_find->text().trimmed().isEmpty()){
        QString findText = ui->lineEdit_find->text();
        QStringList dataFind = ProductManager::findProduct(findText);
        if(dataFind.empty()){
            QMessageBox::information(
                nullptr,
                "Информация",
                "Ничего не найдено по вашему запросу!"
                );
        }else{
            addDataInTable(dataFind);
            flagChangeTable = true;
        }
    }else return;
}

void BasicForm::on_pushButton_deleteFind_clicked(){
    if(!ui->lineEdit_find->text().trimmed().isEmpty() || flagChangeTable){
        ui->lineEdit_find->clear();
        updateDate();
        flagChangeTable = false;
    }
}

void BasicForm::addDataInTable(QStringList products){
    QWidget *scrollContent = ui->scrollArea->widget();
    if (!scrollContent) {
        qCritical() << "Error: ui->scrollArea->widget() is NULL. Cannot display content.";
        return;
    }

    QVBoxLayout *layout = qobject_cast<QVBoxLayout*>(scrollContent->layout());
    if (!layout) {
        layout = new QVBoxLayout(scrollContent);
        scrollContent->setLayout(layout);
    }

    if (products.isEmpty()) {
        clearScrollArea();

        if (m_labelNotFound.isNull()) {
            QLabel *label = new QLabel("Не найдены товары", scrollContent);
            label->setAlignment(Qt::AlignCenter);
            label->setStyleSheet("font-size: 18px; color: #888; padding: 50px;");
            layout->addWidget(label);
            m_labelNotFound = label;
        }
        return;
    }

    if (!m_labelNotFound.isNull()) {
        m_labelNotFound->deleteLater();
        m_labelNotFound = nullptr;
    }

    if (m_tableProducts.isNull()) {
        QTableWidget *table = new QTableWidget(scrollContent);
        layout->addWidget(table);
        m_tableProducts = table;

        table->setSortingEnabled(true);

        QFont font("Leelawadee", 11);
        table->setFont(font);

        table->verticalHeader()->setVisible(false);

        table->setColumnCount(6);
        QStringList headers;
        headers << "Артикул" << "Имя продукта" << "Количество" << "Цена (ед.)" << "Цена остатка" << "Действия";
        table->setHorizontalHeaderLabels(headers);

        QHeaderView *header = table->horizontalHeader();

        header->setSectionResizeMode(QHeaderView::Interactive);

        header->setSectionResizeMode(0, QHeaderView::Stretch);
        header->setSectionResizeMode(1, QHeaderView::Stretch);
        header->setSectionResizeMode(2, QHeaderView::Stretch);
        header->setSectionResizeMode(3, QHeaderView::Stretch);
        header->setSectionResizeMode(4, QHeaderView::Stretch);

        header->setSectionResizeMode(5, QHeaderView::Fixed);

        table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    }

    QTableWidget *table = m_tableProducts;

    table->setColumnWidth(5, 360);

    table->clearContents();
    table->setRowCount(0);

    QSize minButtonSize(100, 30);
    QSize minContainerSize(350, 0);

    QSet<qint64> existingProductIds;

    for (int i = 0; i < products.length(); i++) {
        QStringList parts = products[i].split(" | ", Qt::SkipEmptyParts);

        // getAllProducts() даёт 5 полей (id, name, quantity, price, about),
        // findProduct() — 11 (с движениями). Таблице нужны только первые четыре.
        if (parts.length() < 5) {
            qCritical() << "Неверное количество полей (ожидалось минимум 5) в строке:" << products[i];
            continue;
        }

        bool okID, okQty, okPrice;

        qint64 id = parts[0].contains(": ")
                        ? parts[0].split(": ").last().toLongLong(&okID)
                        : parts[0].toLongLong(&okID);

        QString name = parts[1].trimmed();
        int quantity = parts[2].trimmed().toInt(&okQty);
        double price = parts[3].trimmed().toDouble(&okPrice);

        if (!okID || !okQty || !okPrice) {
            qCritical() << "Ошибка парсинга числовых данных для строки:" << products[i];
            continue;
        }
        if (existingProductIds.contains(id)) {
            qWarning() << "Товар с артикулом (ID) " << id << " уже был добавлен. Пропускаем.";
            continue;
        }
        existingProductIds.insert(id);

        double total_price = quantity * price;

        int row = table->rowCount();
        table->insertRow(row);

        QTableWidgetItem *itemId = new QTableWidgetItem(QString::number(id));
        table->setItem(row, 0, itemId);

        QTableWidgetItem *itemName = new QTableWidgetItem(name);
        table->setItem(row, 1, itemName);

        QTableWidgetItem *itemQty = new QTableWidgetItem(QString::number(quantity));
        table->setItem(row, 2, itemQty);

        QTableWidgetItem *itemPrice = new QTableWidgetItem(QString::number(price, 'f', 2));
        table->setItem(row, 3, itemPrice);

        QTableWidgetItem *itemTotal = new QTableWidgetItem(QString::number(total_price, 'f', 2));
        table->setItem(row, 4, itemTotal);

        QPushButton *deleteButton = new QPushButton("Удалить");
        QPushButton *editButton = new QPushButton("Изменить");

        connect(deleteButton, &QPushButton::clicked, [this, id]() {
            this->on_deleteButton_clicked(id);
        });
        connect(editButton, &QPushButton::clicked, [this, id]() {
            this->on_editButton_clicked(id);
        });

        QWidget *pWidget = new QWidget();
        pWidget->setMinimumSize(minContainerSize);

        QHBoxLayout *pLayout = new QHBoxLayout(pWidget);
        pLayout->addWidget(editButton);
        pLayout->addWidget(deleteButton);

        pLayout->setAlignment(Qt::AlignCenter);
        pLayout->setContentsMargins(0, 0, 0, 0);
        pLayout->setSpacing(5);

        table->setCellWidget(row, 5, pWidget);

        table->setRowHeight(row, minButtonSize.height());

        table->resizeRowToContents(row);
    }

    table->show();
}
