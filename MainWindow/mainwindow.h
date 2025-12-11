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
#include "../EditPage/editform.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    static LoginForm* getLoginForm();
    static BasicForm* getBasicForm();
    static AddForm* getAddForm();
    static EditForm* getEditForm();

private slots:
    void switchToBasicForm();
    void switchToAddForm();
    void cancelFromAdd();
    void switchToEditForm(qint64 id);
    void cancelFromEdit();

private:
    QStackedWidget *stackedWidget;
    int basicIndex;
    int loginIndex;
    int addIndex;
    int editIndex;
    static LoginForm *loginForm;
    static BasicForm *basicForm;
    static AddForm *addForm;
    static EditForm *editForm;

};

#endif // MAINWINDOW_H
