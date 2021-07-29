#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QSignalMapper>
#include <QDebug>
#include <QPushButton>
#include <QSpacerItem>
#include <stdio.h>
#include "diode.h"
#include "custombutton.h"
#include "btn.h"
#include "textsender.h"
#include "textreceiver.h"
#include "message.h"
#include "sender.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:
    void Test(bool);

private:
    Diode* my_diode;
    //CustomButton* my_custombutton;
    QWidget* my_mainWidget;
    QVBoxLayout* my_Vlayout;
    QVBoxLayout* my_Vlayout2;
    QVBoxLayout* my_Vlayout3;
    QHBoxLayout* my_Hlayout;
    QSignalMapper* signalMapper;
};
#endif // MAINWINDOW_H
