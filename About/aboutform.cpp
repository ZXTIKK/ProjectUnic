#include "aboutform.h"
#include "ui_aboutform.h"

#include <QScrollArea>
#include <QVBoxLayout>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QLabel>
#include <QHeaderView>
#include <QMap>
#include <QDate>
#include <QFrame>
#include <QFileDialog>
#include <QStandardPaths>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QDateTime>
#include <QBuffer>
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

static QMap<QDate, double> parseGraphSeries(const QString &series)
{
    QMap<QDate, double> result;
    const QStringList entries = series.split('|', Qt::SkipEmptyParts);
    for (const QString &entry : entries) {
        const int sep = entry.indexOf(':');
        if (sep <= 0) continue;

        const QDate date = QDate::fromString(entry.left(sep), "yyyy-MM-dd");
        bool ok = false;
        const double qty = entry.mid(sep + 1).toDouble(&ok);
        if (date.isValid() && ok && qty > 0)
            result[date] += qty;
    }
    return result;
}

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

void AboutForm::addSupplyAndShipmentCharts(QVBoxLayout *productLayout,
                                           const QMap<QDate, double> &supplies,
                                           const QMap<QDate, double> &shipments,
                                           double currentStock,
                                           qint64 articleNumber)
{
    if (supplies.isEmpty() && shipments.isEmpty()) {
        productLayout->addWidget(new QLabel("<i>Нет движений по товару</i>"));
        return;
    }

    QDate start = QDate::currentDate();
    QDate end   = QDate::currentDate();

    for (const QDate &d : supplies.keys())  { start = qMin(start, d); end = qMax(end, d); }
    for (const QDate &d : shipments.keys()) { start = qMin(start, d); end = qMax(end, d); }

    QVector<QDate> days;
    for (QDate d = start; d <= end; d = d.addDays(1)) {
        days << d;
    }

    double stock = currentStock;

    QMap<QDate, double> stockLevel;
    for (const QDate &d : days) {
        stock += supplies.value(d, 0);
        stock -= shipments.value(d, 0);
        stockLevel[d] = stock;
    }

    QFrame *box = new QFrame();
    box->setStyleSheet(R"(
        QFrame { background: qlineargradient(x1:0,y:0,x2:0,y2:1, stop:0 #fdfdfd, stop:1 #f0f8ff);
                 border-radius: 20px; border: 2px solid #3498db; padding: 14px; margin: 10px; }
    )");
    QVBoxLayout *mainLay = new QVBoxLayout(box);

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

void AboutForm::generateAbout()
{
    QVBoxLayout *mainLayout = qobject_cast<QVBoxLayout*>(ui->scrollArea->widget()->layout());
    if (!mainLayout) {
        qCritical() << "Internal error: mainLayout not found";
        return;
    }

    while (QLayoutItem *item = mainLayout->takeAt(0)) {
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }

    QStringList allProductsData = ProductManager::getAllProducts();
    qDebug() << "Начало генерации AboutForm. Товаров:" << allProductsData.size();

    if (allProductsData.isEmpty()) {
        mainLayout->addWidget(new QLabel("Нет данных о продуктах."));
        mainLayout->addStretch();
        return;
    }

    for (const QString &record : allProductsData) {
        QString idStr = record.section(':', 1, 1).section('|', 0, 0).trimmed();
        qint64 productId = idStr.toLongLong();

        QStringList data = ProductManager::getProductDataById(productId);
        if (data.size() < 9) {
            qWarning() << "Пропуск товара" << productId << "— недостаточно данных:" << data.size();
            continue;
        }

        const QString productName = data[1];
        const QString about       = data[2];
        const QString price       = data[3];
        const QString quantity    = data[4];

        QWidget *productBlock = new QWidget();
        QVBoxLayout *productLayout = new QVBoxLayout(productBlock);
        productBlock->setLayout(productLayout);

        productBlock->setObjectName("ProductBlock" + QString::number(productId));
        productBlock->setStyleSheet("#" + productBlock->objectName() +
                                    " { border: 1px solid #CCCCCC; padding: 10px; margin: 10px; }");

        QLabel *titleLabel = new QLabel(QString("<h3>Продукт ID %1: %2</h3>").arg(productId).arg(productName));
        productLayout->addWidget(titleLabel);

        QStringList productInfoHeaders = {"Название", "Кол-во", "Цена", "Описание"};
        QString productData = QString("%1|%2|%3|%4").arg(productName, quantity, price, about);
        QLabel *infoLabel = new QLabel("<h4>Основная информация</h4>");
        productLayout->addWidget(infoLabel);
        QTableWidget *infoTable = createTable(productInfoHeaders, {productData}, 4);
        productLayout->addWidget(infoTable);

        QStringList suppliesHeaders = {"Дата поставки", "Кол-во", "Поставщик"};
        QStringList suppliesData = data[6].split('#', Qt::SkipEmptyParts);
        QLabel *suppliesLabel = new QLabel("<h4>Поставки</h4>");
        productLayout->addWidget(suppliesLabel);
        if (!suppliesData.isEmpty()) {
            QTableWidget *table = createTable(suppliesHeaders, suppliesData, 3);
            productLayout->addWidget(table);
        } else {
            productLayout->addWidget(new QLabel("<i>Нет записей о поставках.</i>"));
        }

        QStringList shipmentsHeaders = {"Дата отгрузки", "Кол-во", "Получатель"};
        QStringList shipmentsData = data[8].split('#', Qt::SkipEmptyParts);
        QLabel *shipmentsLabel = new QLabel("<h4>Отгрузки</h4>");
        productLayout->addWidget(shipmentsLabel);
        if (!shipmentsData.isEmpty()) {
            QTableWidget *table = createTable(shipmentsHeaders, shipmentsData, 3);
            productLayout->addWidget(table);
        } else {
            productLayout->addWidget(new QLabel("<i>Нет записей об отгрузках.</i>"));
        }

        QMap<QDate, double> supplies  = parseGraphSeries(data[5]);
        QMap<QDate, double> shipments = parseGraphSeries(data[7]);
        addSupplyAndShipmentCharts(productLayout, supplies, shipments, quantity.toDouble(), productId);

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

    QList<QCustomPlot*> allPlots = ui->scrollArea->widget()->findChildren<QCustomPlot*>();

    if (allPlots.isEmpty()) {
        QMessageBox::information(this, "Готово", "Отчёт сохранён без графиков (нет данных для экспорта).");
        return;
    }

    QFile file(fullHtmlPath);
    if (file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        QString content = QString::fromUtf8(file.readAll());

        QString imagesHtml = R"(
            <hr style="border: 2px solid #3498db; margin: 40px 0;">
            <h2 style="text-align:center; color:#2c3e50; font-family: Arial;">Графики движения по всем артикулам</h2>
            <p style="text-align:center; color:#7f8c8d;">Всего графиков: )" + QString::number(allPlots.size()) + "</p>";

        for (QCustomPlot *plot : allPlots) {
            QBuffer buffer;
            buffer.open(QIODevice::WriteOnly);
            plot->toPixmap().save(&buffer, "PNG");

            QString base64Image = QString::fromLatin1(buffer.data().toBase64());

            imagesHtml += QString(R"(
                <div style="page-break-inside: avoid; margin: 40px auto; text-align:center;">
                    <img src="data:image/png;base64,%1" style="max-width:95%; border:3px solid #3498db; border-radius:16px; box-shadow: 0 8px 20px rgba(0,0,0,0.15);">
                </div>
            )").arg(base64Image);
        }

        content.replace("</body>", imagesHtml + "</body>");
        file.resize(0);
        file.write(content.toUtf8());
        file.close();

        QMessageBox::information(this, "Готово!",
                                 QString("Полный отчёт по <b>всем артикулам</b> сохранён!\n\n"
                                         "• Таблица товаров\n"
                                         "• %1 график(ов) (встроены в файл)\n\n"
                                         "%2")
                                     .arg(allPlots.size())
                                     .arg(fullHtmlPath));
    }
}