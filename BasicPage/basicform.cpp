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
    QStringList products = ProductManager::getAllProducts();
    addDataInTable(products);
}

void BasicForm::on_detailsButton_clicked(qint64 id)
{
    qDebug() << "Нажата кнопка 'Подробнее' для продукта с ID:" << id;
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
        }
    }else return;
}

void BasicForm::on_pushButton_deleteFind_clicked(){
    if(!ui->lineEdit_find->text().trimmed().isEmpty()){
        ui->lineEdit_find->clear();
        updateDate();
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

        header->setSectionResizeMode(QHeaderView::Stretch);

        header->setSectionResizeMode(5, QHeaderView::ResizeToContents);

        header->setSectionResizeMode(0, QHeaderView::Stretch);
        header->setSectionResizeMode(1, QHeaderView::Stretch);
        header->setSectionResizeMode(2, QHeaderView::Stretch);
        header->setSectionResizeMode(3, QHeaderView::Stretch);
        header->setSectionResizeMode(4, QHeaderView::Stretch);

        table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    }

    QTableWidget *table = m_tableProducts;

    table->clearContents();
    table->setRowCount(0);

    QSize minButtonSize(100, 30);
    QSize minContainerSize(320, 0);

    for (int i = 0; i < products.length(); i++) {
        QStringList parts = products[i].split(" | ", Qt::SkipEmptyParts);

        if (parts.length() < 11) {
            qCritical() << "Неверное количество полей (ожидалось 11) в строке:" << products[i];
            continue;
        }

        bool okID, okQty, okPrice;

        qint64 id = parts[0].split(": ").last().toLongLong(&okID);
        QString name = parts[1].trimmed();
        int quantity = parts[2].trimmed().toInt(&okQty);
        double price = parts[3].trimmed().toDouble(&okPrice);

        if (!okID || !okQty || !okPrice) {
            qCritical() << "Ошибка парсинга числовых данных для строки:" << products[i];
            continue;
        }

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

        QPushButton *detailsButton = new QPushButton("Подробнее");
        QPushButton *deleteButton = new QPushButton("Удалить");
        QPushButton *editButton = new QPushButton("Изменить");

        detailsButton->setMinimumSize(minButtonSize);
        deleteButton->setMinimumSize(minButtonSize);
        editButton->setMinimumSize(minButtonSize);

        detailsButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        deleteButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        editButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

        connect(detailsButton, &QPushButton::clicked, [this, id]() {
            this->on_detailsButton_clicked(id);
        });
        connect(deleteButton, &QPushButton::clicked, [this, id]() {
            this->on_deleteButton_clicked(id);
        });
        connect(editButton, &QPushButton::clicked, [this, id]() {
            this->on_editButton_clicked(id);
        });

        QWidget *pWidget = new QWidget();
        pWidget->setMinimumSize(minContainerSize);

        QHBoxLayout *pLayout = new QHBoxLayout(pWidget);
        pLayout->addWidget(detailsButton);
        pLayout->addWidget(editButton);
        pLayout->addWidget(deleteButton);

        pLayout->setAlignment(Qt::AlignCenter);
        pLayout->setContentsMargins(0, 0, 0, 0);

        table->setCellWidget(row, 5, pWidget);
    }

    table->show();
}

