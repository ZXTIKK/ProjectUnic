#include "MainWindow/mainwindow.h"
#include <QApplication>
#include <QIcon>
#include <cstdio>
#include <iostream>

int main(int argc, char *argv[])
{
#ifdef NDEBUG
    std::freopen("nul", "w", stdout);
    std::freopen("nul", "w", stderr);
    std::ios_base::sync_with_stdio(false);
#endif
    QApplication a(argc, argv);

    a.setWindowIcon(QIcon("/Res/iconApp.png"));

    MainWindow w;
    w.show();

    return a.exec();
}