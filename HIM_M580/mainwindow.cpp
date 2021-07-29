#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("M580 HIM");

    my_mainWidget = new QWidget(this);
    my_Vlayout = new QVBoxLayout(this);
//    my_Vlayout2 = new QVBoxLayout(this);
//    my_Vlayout3 = new QVBoxLayout(this);
    my_Hlayout = new QHBoxLayout(this);

    my_diode = new Diode(my_mainWidget);

    //my_mainWidget->setStyleSheet("border: 1px solid rgb( 199, 0, 57)");

    QPushButton *diode_button = new QPushButton(tr("&Power Diode"),my_mainWidget);
    diode_button->setStyleSheet("background-color: rgb(237, 187, 153);"
                              "color: rgb(255, 255, 255);");

    //CustomButton* my_custombutton = new CustomButton("Power Diode",my_mainWidget);

    connect(diode_button, SIGNAL(clicked()), my_diode, SLOT(setPower()) );

    my_Hlayout->addLayout(my_Vlayout, 20);
    my_Vlayout->addWidget(my_diode);
    QSpacerItem * horizontalSpacer = new QSpacerItem(20, 100, QSizePolicy::Expanding, QSizePolicy::Minimum);
    my_Vlayout->addItem(horizontalSpacer);
    my_Vlayout->addWidget(diode_button);

    Sender * my_sender = new Sender();
    my_Hlayout->addWidget(my_sender);

//    my_Hlayout->addLayout(my_Vlayout2,40);
//    my_Hlayout->addLayout(my_Vlayout3,40);

//    //signalMapper = new QSignalMapper(this);
//    /******************************SENDER**********************************/
//    TextSender* sender = new TextSender(my_mainWidget);
//    QStringList headers;

// //    headers << "Message envoyé par victor"
// //            << "Au M580";
// //    QString body = "This is a test.\r\n";

// //    Message message(body, headers);
// //_____________________________________________________________________
//    TextReceiver* receiver = new TextReceiver(my_mainWidget);
// //    receiver->setMessage(message);

//    QObject::connect(sender, SIGNAL(messageSent(Message)),
//                     receiver, SLOT(setMessage(Message)));

// //    QObject::connect(receiver, SIGNAL(messageSent(Message)),
// //                     sender, SLOT(setMessage(Message)));
//  //_____________________________________________________________________
//    my_Vlayout2->addWidget(sender);
//    my_Vlayout3->addWidget(receiver);

    my_mainWidget->setLayout(my_Hlayout);
    my_mainWidget->setStyleSheet("background-color: rgb(254, 245, 231)");



    setCentralWidget(my_mainWidget);


}


void MainWindow::Test(bool b)
{
    qDebug() << "Test Reussi!" ;
}

MainWindow::~MainWindow()
{
    qDebug() << "Exit!" ;
}
