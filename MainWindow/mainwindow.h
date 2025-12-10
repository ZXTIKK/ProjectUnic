// mainwindow.h
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QStackedWidget>

#include "../LoginPage/loginform.h"
#include "../BasicPage/basicform.h"
#include "../AddPage/addform.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    static LoginForm* getLoginForm();
    static BasicForm* getBasicForm();
    static AddForm* getAddForm();

private slots:
    void switchToBasicForm();
    void switchToAddForm();
    void cancelFromAdd();

private:
    QStackedWidget *stackedWidget;
    int basicIndex;
    int loginIndex;
    int addIndex;
    static LoginForm *loginForm;
    static BasicForm *basicForm;
    static AddForm *addForm;


};

#endif // MAINWINDOW_H
