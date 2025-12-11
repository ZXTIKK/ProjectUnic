// settingsform.cpp

#include "settingsform.h"
#include "ui_settingsform.h"

#include <QSettings>
#include <QDebug>
#include <QMessageBox> // Добавлено для обработки ошибок

QString SettingsForm::m_serverAddress = "127.0.0.1";
quint16 SettingsForm::m_serverPort = 8080;

QString SettingsForm::serverAddress() {
    return m_serverAddress;
}

quint16 SettingsForm::serverPort() {
    return m_serverPort;
}

bool SettingsForm::loadSettingsFromRegistry() {

    QSettings settings("MyCompanyName", "MyInventoryApp");

    if (!settings.contains("Server/Address") || !settings.contains("Server/Port")) {
        qDebug() << "Settings not found in registry. Using default values.";
        return false;
    }

    m_serverAddress = settings.value("Server/Address", m_serverAddress).toString();
    m_serverPort = settings.value("Server/Port", m_serverPort).toUInt();

    if (m_serverPort == 0) {
        qWarning() << "Invalid port read from registry. Using default port.";
        m_serverPort = 8080;
    }

    qDebug() << "Settings loaded successfully from registry:";
    qDebug() << "  Address:" << m_serverAddress;
    qDebug() << "  Port:" << m_serverPort;

    return true;
}


void SettingsForm::saveOrUpdateSettings(const QString& address, quint16 port) {

    QSettings settings("MyCompanyName", "MyInventoryApp");
    m_serverAddress = address;
    m_serverPort = port;

    settings.setValue("Server/Address", address);
    settings.setValue("Server/Port", port);

    settings.sync();

    qDebug() << "Settings saved/updated in registry:";
    qDebug() << "  New Address:" << m_serverAddress;
    qDebug() << "  New Port:" << m_serverPort;
}

SettingsForm::SettingsForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SettingsForm)
{
    ui->setupUi(this);

    loadSettingsFromRegistry();
    ui->lineEdit_Adress->setText(m_serverAddress);

}

SettingsForm::~SettingsForm()
{
    delete ui;
}

void SettingsForm::on_pushButton_back_clicked()
{
    emit Cancel_Settings();
}

void SettingsForm::on_pushButton_conf_clicked()
{
    QString address = ui->lineEdit_Adress->text().trimmed();
    quint16 port = 0;

    bool ok;
    port = static_cast<quint16>(ui->spinBox_port->value());

    if (!ok || port == 0) {
        QMessageBox::critical(this, "Ошибка ввода", "Введите корректный номер порта (1 - 65535).");
        return;
    }

    if (address.isEmpty()) {
        QMessageBox::critical(this, "Ошибка ввода", "Поле адреса сервера не может быть пустым.");
        return;
    }

    saveOrUpdateSettings(address, port);
\
    emit Cancel_Settings();
}
