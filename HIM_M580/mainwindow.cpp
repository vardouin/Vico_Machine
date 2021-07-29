#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("M580 HIM");

    my_mainWidget = new QWidget(this);
    my_Vlayout = new QVBoxLayout(this);
    my_Hlayout = new QHBoxLayout(this);

    my_diode = new Diode(my_mainWidget);

    QPushButton *diode_button = new QPushButton(tr("&Power Diode"),my_mainWidget);
    diode_button->setStyleSheet("background-color: rgb(237, 187, 153);"
                              "color: rgb(255, 255, 255);");

    connect(diode_button, SIGNAL(clicked()), my_diode, SLOT(setPower()) );

    my_Hlayout->addLayout(my_Vlayout, 20);
    my_Vlayout->addWidget(my_diode);
    QSpacerItem * horizontalSpacer = new QSpacerItem(20, 100, QSizePolicy::Expanding, QSizePolicy::Minimum);
    my_Vlayout->addItem(horizontalSpacer);
    my_Vlayout->addWidget(diode_button);

    Sender * my_sender = new Sender();
    my_Hlayout->addWidget(my_sender);

    my_mainWidget->setLayout(my_Hlayout);
    my_mainWidget->setStyleSheet("background-color: rgb(254, 245, 231)");

    setCentralWidget(my_mainWidget);

}

MainWindow::~MainWindow()
{
    qDebug() << "Exit!" ;
}
