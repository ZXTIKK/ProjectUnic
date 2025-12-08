#include "mainwindow.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QMessageBox>
#include <QDebug>
#include <QStackedWidget>

#include "../LoginPage/loginform.h"
#include "../BasicPage/basicform.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);

    LoginForm *loginForm = new LoginForm();
    BasicForm *basicForm = new BasicForm();

    int loginIndex = stackedWidget->addWidget(loginForm);
    basicIndex = stackedWidget->addWidget(basicForm);

    stackedWidget->setCurrentIndex(loginIndex);
    setWindowTitle("Авторизация");

    connect(loginForm, &LoginForm::loginSuccessful,
            this, &MainWindow::switchToBasicForm);

}

MainWindow::~MainWindow() {}

void MainWindow::switchToBasicForm()
{
    stackedWidget->setCurrentIndex(basicIndex);
    setWindowTitle("Товары");
}



