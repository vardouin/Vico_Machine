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
#include "sender.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

private:
    Diode* my_diode;
    QWidget* my_mainWidget;
    QVBoxLayout* my_Vlayout;
    QHBoxLayout* my_Hlayout;
    QSignalMapper* signalMapper;
};
#endif // MAINWINDOW_H
