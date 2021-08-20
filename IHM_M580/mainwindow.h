#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QSignalMapper>
#include <QDebug>
#include <QPushButton>
#include <QSpacerItem>
#include <QProcess>
#include <stdio.h>
#include "diode.h"
#include "sender.h"
#include "m580_button.h"
#include "valveusecase.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

public slots:
    void launchbash();
    void killM580();

private:
    Diode* my_diode;
    Sender * my_sender;
    QProcess *process;
    QWidget* my_mainWidget;
    QVBoxLayout* my_Vlayout0;
    QVBoxLayout* my_Vlayout;
    QVBoxLayout* my_Vlayout2;
    QHBoxLayout* my_Hlayout;
    QString path = "/home/vardouin/Project/qemu/launch.sh";
    qint64 id;
};
#endif // MAINWINDOW_H
