#include "mainwindow.h"
#include "sender.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


//    window1.show();
//    window2.show();


    MainWindow w;
    w.show();

//    Sender w;
//    w.show();

    return a.exec();
}
