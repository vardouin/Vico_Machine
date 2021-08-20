#include "mainwindow.h"
#include "sender.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    w.show();

//    ValveUsecase v;
//    v.show();

//    Sender w;
//    w.show();

    return a.exec();
}
