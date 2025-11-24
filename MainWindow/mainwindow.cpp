#include "mainwindow.h"
#include "../BusinessLogic/authentication.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QMessageBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , lineEditLogin(nullptr)
    , lineEditPassword(nullptr)
    , lableInfo(nullptr)
{
    setWindowTitle("Авторизация");
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(30, 30, 30, 30);
    layout->setSpacing(15);
    layout->setAlignment(Qt::AlignCenter);

    QLabel* labelAuth = new QLabel("ВХОД");
    labelAuth->setAlignment(Qt::AlignCenter);
    labelAuth->setStyleSheet("font-size: 25px; font-weight: bold; margin-bottom: 50px;");

    lineEditLogin = new QLineEdit();
    lineEditLogin->setPlaceholderText("Логин");
    lineEditLogin->setMinimumWidth(200);
    lineEditLogin->setMaximumWidth(300);
    lineEditLogin->setStyleSheet("QLineEdit { border: 1px solid grey; border-radius: 5px; padding: 5px; }");

    lineEditPassword = new QLineEdit();
    lineEditPassword->setEchoMode(QLineEdit::Password);
    lineEditPassword->setPlaceholderText("Пароль");
    lineEditPassword->setMinimumWidth(200);
    lineEditPassword->setMaximumWidth(300);
    lineEditPassword->setStyleSheet("QLineEdit { border: 1px solid grey; border-radius: 5px; padding: 5px; }");

    lableInfo = new QLabel();
    lableInfo->setAlignment(Qt::AlignCenter);
    lableInfo->setStyleSheet("font-size: 12px; font-weight: bold; margin-bottom: 10px; color:red;");

    QPushButton* btnAuth = new QPushButton("ВОЙТИ");
    btnAuth->setMinimumHeight(40);
    btnAuth->setMaximumWidth(300);

    layout->addWidget(labelAuth);
    layout->addWidget(lineEditLogin);
    layout->addWidget(lineEditPassword);
    layout->addWidget(lableInfo);
    layout->addWidget(btnAuth);


    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    connect(btnAuth, &QPushButton::clicked, this, &MainWindow::onLoginButtonClicked);
    connect(lineEditPassword, &QLineEdit::returnPressed, this, &MainWindow::onLoginButtonClicked);
}

MainWindow::~MainWindow() {}


void MainWindow::onLoginButtonClicked()
{
    QString login = lineEditLogin->text().trimmed();
    QString password = lineEditPassword->text();

    bool loginValid = !login.isEmpty();
    bool passwordValid = !password.isEmpty();

    updateLineEditStyle(lineEditLogin, loginValid);
    updateLineEditStyle(lineEditPassword, passwordValid);

    if (!loginValid || !passwordValid) {
        lableInfo->setText("");
        return;
    }

    bool success = Authentication::authentication(login, password);

    if (success) {
        lableInfo->setText("");
        QMessageBox::information(this, "Успех", "Вы успешно вошли в систему!");
    } else {
        lableInfo->setText("Неверный логин или пароль!");
        lineEditPassword->clear();
        lineEditPassword->setFocus();
    }
}

void MainWindow::updateLineEditStyle(QLineEdit* edit, bool isValid)
{
    if (isValid) {
        edit->setStyleSheet("QLineEdit { border: 1px solid grey; border-radius: 5px; padding: 5px; }");
    } else {
        edit->setStyleSheet("QLineEdit { border: 1px solid red; border-radius: 5px; padding: 5px; }");
    }
}
