// mainwindow.h
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QStackedWidget>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void switchToBasicForm();

private:
    QStackedWidget *stackedWidget; // <-- Сделайте его членом класса
    int basicIndex;
    int loginIndex;

};

#endif // MAINWINDOW_H
