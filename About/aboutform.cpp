#include "aboutform.h"
#include "ui_aboutform.h"

#include <QScrollArea>
#include <QVBoxLayout>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QLabel>
#include <QHeaderView>
#include "../BusinessLogic/QCustomPlot/qcustomplot.h"

#include "../MainWindow/mainwindow.h"
#include "../BasicPage/basicform.h"
#include "../BusinessLogic/ProductManager/productmanager.h"
#include "../BusinessLogic/OutputWord/outputword.h"
#include "../BusinessLogic/OutputExcel/outputexcel.h"

AboutForm::AboutForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AboutForm)
{
    ui->setupUi(this);

    QWidget *contentWidget = new QWidget(ui->scrollArea);
    QVBoxLayout *mainLayout = new QVBoxLayout(contentWidget);
    mainLayout->setAlignment(Qt::AlignTop);
    contentWidget->setLayout(mainLayout);

    ui->scrollArea->setWidgetResizable(true);
    ui->scrollArea->setWidget(contentWidget);

    connect(MainWindow::getBasicForm(), &BasicForm::switchToAbout,
            this, &AboutForm::generateAbout);
}

AboutForm::~AboutForm()
{
    delete ui;
}

void AboutForm::on_pushButton_back_clicked()
{
    emit cancelAbout();
}

// Вспомогательная функция: создаёт QTableWidget
QTableWidget* createTable(const QStringList &headers, const QStringList &data, int columnsCount)
{
    QTableWidget *table = new QTableWidget();
    table->setColumnCount(columnsCount);
    table->setHorizontalHeaderLabels(headers);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setRowCount(data.size());

    for (int row = 0; row < data.size(); ++row) {
        QStringList fields = data.at(row).split('|', Qt::SkipEmptyParts);
        for (int col = 0; col < columnsCount && col < fields.size(); ++col) {
            QTableWidgetItem *item = new QTableWidgetItem(fields.at(col).trimmed());
            table->setItem(row, col, item);
        }
    }

    table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    table->verticalHeader()->setVisible(false);
    table->setMinimumHeight(table->rowHeight(0) * (table->rowCount() + 2));

    return table;
}

// Новая функция: добавляет два графика — поставки и отгрузки
void AboutForm::addSupplyAndShipmentCharts(QVBoxLayout *productLayout, const QStringList &records, qint64 articleNumber)
{
    QMap<QDate, double> supplies;
    QMap<QDate, double> shipments;

    for (const QString &record : records) {
        QStringList f = record.split('|', Qt::SkipEmptyParts);

        // Поставки
        if (f.size() > 10) {
            QString d = f[8].trimmed();
            double q = f[9].toDouble();
            if (q > 0 && d != "N/A") {
                QDate date = QDate::fromString(d, "yyyy-MM-dd");
                if (date.isValid()) supplies[date] += q;
            }
        }

        // Отгрузки
        if (f.size() > 7) {
            QString d = f[5].trimmed();
            double q = f[6].toDouble();
            if (q > 0 && d != "N/A") {
                QDate date = QDate::fromString(d, "yyyy-MM-dd");
                if (date.isValid()) shipments[date] += q;
            }
        }
    }

    if (supplies.isEmpty() && shipments.isEmpty()) {
        productLayout->addWidget(new QLabel("<i>Нет движений по товару</i>"));
        return;
    }

    QDate start = QDate::currentDate();
    QDate end   = QDate::currentDate();

    for (QDate d : supplies.keys())  { start = qMin(start, d); end = qMax(end, d); }
    for (QDate d : shipments.keys()) { start = qMin(start, d); end = qMax(end, d); }

    QVector<QDate> days;
    for (QDate d = start; d <= end; d = d.addDays(1)) {
        days << d;
    }

    double stock = 0.0;
    if (!records.isEmpty()) {
        QStringList p = records.first().split('|', Qt::SkipEmptyParts);
        if (p.size() > 2) stock = p[2].toDouble();
    }

    QMap<QDate, double> stockLevel;
    for (const QDate &d : days) {
        stock += supplies.value(d, 0);
        stock -= shipments.value(d, 0);
        stockLevel[d] = stock;
    }

    // Красивый контейнер
    QFrame *box = new QFrame();
    box->setStyleSheet(R"(
        QFrame { background: qlineargradient(x1:0,y:0,x2:0,y2:1, stop:0 #fdfdfd, stop:1 #f0f8ff);
                 border-radius: 20px; border: 2px solid #3498db; padding: 14px; margin: 10px; }
    )");
    QVBoxLayout *mainLay = new QVBoxLayout(box);

    // Заголовок — теперь только «Артикул»
    QLabel *header = new QLabel(QString(
                                    "<h3 style='color:#2c3e50; margin:12px; font-weight:bold;'>"
                                    "Артикул: <span style='color:#e74c3c; font-size:18pt;'>%1</span><br>"
                                    "<span style='color:#7f8c8d; font-size:12pt;'>%2 — %3</span></h3>")
                                    .arg(articleNumber)
                                    .arg(start.toString("dd.MM.yyyy"))
                                    .arg(QDate::currentDate().toString("dd.MM.yyyy"))
                                );
    header->setAlignment(Qt::AlignCenter);
    mainLay->addWidget(header);

    auto dateTicker = QSharedPointer<QCPAxisTickerDateTime>(new QCPAxisTickerDateTime);
    dateTicker->setDateTimeFormat("dd.MM");

    // Остаток
    QCustomPlot *plotStock = new QCustomPlot();
    plotStock->setMinimumHeight(230);
    plotStock->setBackground(QColor(250, 255, 255));

    QCPGraph *line = plotStock->addGraph();
    line->setPen(QPen(QColor("#2980b9"), 4, Qt::SolidLine, Qt::RoundCap));
    line->setBrush(QBrush(QColor(52, 152, 219, 60)));
    line->setName("Остаток на складе");

    QVector<double> sx, sy;
    for (const QDate &d : days) {
        sx << QDateTime(d, QTime(12,0)).toSecsSinceEpoch();
        sy << stockLevel[d];
    }
    line->setData(sx, sy);

    plotStock->xAxis->setTicker(dateTicker);
    plotStock->xAxis->setTickLabelRotation(30);
    plotStock->yAxis->setLabel("Остаток (шт)");
    plotStock->legend->setVisible(true);
    plotStock->rescaleAxes();
    plotStock->replot();
    mainLay->addWidget(plotStock);

    // Сводный график: поставки вверх, отгрузки вниз
    QCustomPlot *plotCombined = new QCustomPlot();
    plotCombined->setMinimumHeight(210);

    QCPBars *barsIn  = new QCPBars(plotCombined->xAxis, plotCombined->yAxis);
    QCPBars *barsOut = new QCPBars(plotCombined->xAxis, plotCombined->yAxis);

    barsIn->setName("Поставки (+)");
    barsIn->setBrush(QColor(39, 174, 96, 240));
    barsIn->setPen(QPen(QColor(27, 133, 74), 2));
    barsIn->setWidth(86400 * 0.8);

    barsOut->setName("Отгрузки (–)");
    barsOut->setBrush(QColor(231, 76, 60, 240));
    barsOut->setPen(QPen(QColor(192, 57, 43), 2));
    barsOut->setWidth(86400 * 0.8);

    QVector<double> inX, inY, outX, outY;
    for (const QDate &d : days) {
        double s = supplies.value(d, 0);
        double h = shipments.value(d, 0);
        if (s > 0) { inX << QDateTime(d, QTime(12,0)).toSecsSinceEpoch(); inY << s; }
        if (h > 0) { outX << QDateTime(d, QTime(12,0)).toSecsSinceEpoch(); outY << -h; }
    }

    barsIn->setData(inX, inY);
    barsOut->setData(outX, outY);

    plotCombined->xAxis->setTicker(dateTicker);
    plotCombined->xAxis->setTickLabelRotation(30);
    plotCombined->yAxis->setLabel("Движение (± шт)");
    plotCombined->legend->setVisible(true);
    plotCombined->rescaleAxes();
    plotCombined->yAxis->setRange(plotCombined->yAxis->range().lower * 1.1,
                                  plotCombined->yAxis->range().upper * 1.1);
    plotCombined->replot();

    QLabel *lbl = new QLabel("<h4 style='color:#8e44ad; text-align:center; margin:10px 0;'>Сводное движение товара</h4>");
    mainLay->addWidget(lbl);
    mainLay->addWidget(plotCombined);

    productLayout->addWidget(box);
}
// Основная функция генерации страницы
void AboutForm::generateAbout()
{
    QVBoxLayout *mainLayout = qobject_cast<QVBoxLayout*>(ui->scrollArea->widget()->layout());
    if (!mainLayout) {
        qCritical() << "Internal error: mainLayout not found";
        return;
    }

    // Очищаем старое содержимое
    while (QLayoutItem *item = mainLayout->takeAt(0)) {
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }

    QStringList allProductsData = ProductManager::getAllProducts();
    qDebug() << "Начало генерации AboutForm. Записей:" << allProductsData.size();

    if (allProductsData.isEmpty()) {
        mainLayout->addWidget(new QLabel("Нет данных о продуктах."));
        mainLayout->addStretch();
        return;
    }

    // Группируем по ID продукта
    QMap<qint64, QStringList> productRecords;
    for (const QString &record : allProductsData) {
        QString idStr = record.section(':', 1, 1).section('|', 0, 0).trimmed();
        qint64 id = idStr.toLongLong();
        productRecords[id].append(record);
    }

    for (qint64 productId : productRecords.keys()) {
        const QStringList &records = productRecords.value(productId);

        QWidget *productBlock = new QWidget();
        QVBoxLayout *productLayout = new QVBoxLayout(productBlock);
        productBlock->setLayout(productLayout);

        productBlock->setObjectName("ProductBlock" + QString::number(productId));
        productBlock->setStyleSheet("#" + productBlock->objectName() +
                                    " { border: 1px solid #CCCCCC; padding: 10px; margin: 10px; }");

        // Название продукта
        QString productName = records.first().section('|', 1, 1).section(':', 0, 0).trimmed();
        QLabel *titleLabel = new QLabel(QString("<h3>Продукт ID %1: %2</h3>").arg(productId).arg(productName));
        productLayout->addWidget(titleLabel);

        // Основная информация
        QStringList productInfoHeaders = {"Название", "Кол-во", "Цена", "Описание"};
        QString productData = records.first().section('|', 1, 4);
        QLabel *infoLabel = new QLabel("<h4>Основная информация</h4>");
        productLayout->addWidget(infoLabel);
        QTableWidget *infoTable = createTable(productInfoHeaders, {productData}, 4);
        productLayout->addWidget(infoTable);

        // Поставки (таблица)
        QStringList suppliesHeaders = {"Дата поставки", "Кол-во", "Поставщик"};
        QStringList suppliesData;
        for (const QString &r : records) {
            QString s = r.section('|', 8, 10);
            if (!s.contains("N/A")) suppliesData.append(s);
        }
        QLabel *suppliesLabel = new QLabel("<h4>Поставки</h4>");
        productLayout->addWidget(suppliesLabel);
        if (!suppliesData.isEmpty()) {
            QTableWidget *table = createTable(suppliesHeaders, suppliesData, 3);
            productLayout->addWidget(table);
        } else {
            productLayout->addWidget(new QLabel("<i>Нет записей о поставках.</i>"));
        }

        // Отгрузки (таблица)
        QStringList shipmentsHeaders = {"Дата отгрузки", "Кол-во", "Получатель"};
        QStringList shipmentsData;
        for (const QString &r : records) {
            QString s = r.section('|', 5, 7);
            if (!s.contains("N/A")) shipmentsData.append(s);
        }
        QLabel *shipmentsLabel = new QLabel("<h4>Отгрузки</h4>");
        productLayout->addWidget(shipmentsLabel);
        if (!shipmentsData.isEmpty()) {
            QTableWidget *table = createTable(shipmentsHeaders, shipmentsData, 3);
            productLayout->addWidget(table);
        } else {
            productLayout->addWidget(new QLabel("<i>Нет записей об отгрузках.</i>"));
        }

        // ←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←
        // ГРАФИКИ ПОСТАВОК И ОТГРУЗОК
        addSupplyAndShipmentCharts(productLayout, records, productId);
        // ←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←

        // Разделитель
        QFrame *line = new QFrame();
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        productLayout->addWidget(line);

        mainLayout->addWidget(productBlock);
    }

    mainLayout->addStretch(1);
    qDebug() << "Генерация AboutForm завершена";
}

void AboutForm::on_pushButton_Excel_clicked()
{
    QString fileName = QString("Полный_отчет_по_всем_артикулам_%1")
                           .arg(QDate::currentDate().toString("dd-MM-yyyy"));

    QString savePath = QFileDialog::getExistingDirectory(
        this,
        "Выберите папку для сохранения Excel (CSV)",
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
        );

    if (savePath.isEmpty()) return;

    OutputExcel exporter;
    bool success = exporter.exportAllToCsv(savePath, fileName);

    if (success) {
        QMessageBox::information(this,
                                 "Успех",
                                 QString("Полный отчёт по всем артикулам успешно экспортирован!\n\n%1/%2.csv")
                                     .arg(savePath, fileName));
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось экспортировать данные в CSV.");
    }
}

void AboutForm::on_pushButton_Word_clicked()
{
    QString fileName = QString("Полный_отчет_по_всем_артикулам_%1")
                           .arg(QDate::currentDate().toString("dd-MM-yyyy"));

    QString savePath = QFileDialog::getExistingDirectory(
        this,
        "Выберите папку для сохранения отчёта Word (HTML",
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
        );

    if (savePath.isEmpty()) return;

    OutputWord wordExporter;
    bool success = wordExporter.exportAll(savePath, fileName);

    if (!success) {
        QMessageBox::warning(this, "Ошибка", "Не удалось создать отчёт Word.");
        return;
    }

    QString fullHtmlPath = QString("%1/%2.html").arg(savePath, fileName);

    // === 2. Добавляем в конец отчёта ВСЕ графики по всем артикулам ===
    QTemporaryDir tempDir;
    if (!tempDir.isValid()) {
        QMessageBox::warning(this, "Ошибка", "Не удалось создать временную папку.");
        return;
    }

    QStringList imagePaths;

    // Собираем ВСЕ QCustomPlot из scrollArea (все артикулы!)
    QList<QCustomPlot*> allPlots = ui->scrollArea->widget()->findChildren<QCustomPlot*>();

    if (allPlots.isEmpty()) {
        QMessageBox::information(this, "Готово", "Отчёт сохранён без графиков (нет данных для экспорта).");
        return;
    }

    for (int i = 0; i < allPlots.size(); ++i) {
        QCustomPlot *plot = allPlots[i];
        QString imgPath = tempDir.path() + QString("/graph_%1.png").arg(i + 1, 3, 10, QChar('0'));
        plot->savePng(imgPath, 0, 0, 1.5, -1); // качество чуть выше
        imagePaths << imgPath;
    }

    // Вставляем все графики в HTML
    QFile file(fullHtmlPath);
    if (file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        QString content = QString::fromUtf8(file.readAll());

        QString imagesHtml = R"(
            <hr style="border: 2px solid #3498db; margin: 40px 0;">
            <h2 style="text-align:center; color:#2c3e50; font-family: Arial;">Графики движения по всем артикулам</h2>
            <p style="text-align:center; color:#7f8c8d;">Всего графиков: )" + QString::number(allPlots.size()) + "</p>";

        for (const QString &img : imagePaths) {
            QString finalImg = savePath + "/" + QFileInfo(img).fileName();
            QFile::copy(img, finalImg);

            imagesHtml += QString(R"(
                <div style="page-break-inside: avoid; margin: 40px auto; text-align:center;">
                    <img src="%1" style="max-width:95%; border:3px solid #3498db; border-radius:16px; box-shadow: 0 8px 20px rgba(0,0,0,0.15);">
                </div>
            )").arg(QFileInfo(finalImg).fileName());
        }

        content.replace("</body>", imagesHtml + "</body>");
        file.resize(0);
        file.write(content.toUtf8());
        file.close();

        QMessageBox::information(this, "Готово!",
                                 QString("Полный отчёт по <b>всем артикулам</b> сохранён!\n\n"
                                         "• Таблица товаров\n"
                                         "• %1 график(ов)\n\n"
                                         "%2/%3.html")
                                     .arg(allPlots.size())
                                     .arg(savePath, fileName));
    }
}
