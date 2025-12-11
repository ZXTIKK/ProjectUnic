// settingsform.h

#ifndef SETTINGSFORM_H
#define SETTINGSFORM_H

#include <QWidget>
#include <QString>
#include <QtGlobal>

namespace Ui {
class SettingsForm;
}

class SettingsForm : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsForm(QWidget *parent = nullptr);
    ~SettingsForm();
    static QString serverAddress();
    static quint16 serverPort();
    static bool loadSettingsFromRegistry();

public slots:
    static void saveOrUpdateSettings(const QString& address, quint16 port);
    void on_pushButton_conf_clicked();

private:
    Ui::SettingsForm *ui;

    static QString m_serverAddress;
    static quint16 m_serverPort;

private slots:
    void on_pushButton_back_clicked();

signals:
    void Cancel_Settings();
};

#endif // SETTINGSFORM_H
