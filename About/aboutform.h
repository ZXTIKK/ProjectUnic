// aboutform.h (дополненная версия)
#ifndef ABOUTFORM_H
#define ABOUTFORM_H

#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QMap>
#include <QDate>

class QCustomPlot;

namespace Ui {
class AboutForm;
}

class AboutForm : public QWidget
{
    Q_OBJECT

public:
    explicit AboutForm(QWidget *parent = nullptr);
    ~AboutForm();

signals:
    void cancelAbout();

private slots:
    void on_pushButton_back_clicked();
    void generateAbout();
    void on_pushButton_Excel_clicked();
    void on_pushButton_Word_clicked();

private:
    Ui::AboutForm *ui;
    void addSupplyAndShipmentCharts(QVBoxLayout *productLayout,
                                    const QMap<QDate, double> &supplies,
                                    const QMap<QDate, double> &shipments,
                                    double currentStock,
                                    qint64 articleNumber);
};

#endif // ABOUTFORM_H
