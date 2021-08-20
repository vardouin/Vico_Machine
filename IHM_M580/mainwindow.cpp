#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    qApp->installEventFilter(this);


    my_mainWidget = new QWidget(this);
    setCentralWidget(my_mainWidget);
    setWindowTitle("M580 IHM");
    my_mainWidget->setStyleSheet("background-color: rgb(254, 245, 231)");

    my_Vlayout0 = new QVBoxLayout;
    my_Hlayout  = new QHBoxLayout;
    my_Vlayout  = new QVBoxLayout;
    my_Vlayout2 = new QVBoxLayout;

    my_diode    = new Diode(my_mainWidget);
    my_sender   = new Sender();
    QSpacerItem * horizontalSpacer  = new QSpacerItem(5, 10, QSizePolicy::Expanding, QSizePolicy::Minimum);
    M580_button * diode_button      = new M580_button(tr("&Power Diode"), my_mainWidget);
    M580_button * shbutton          = new M580_button(tr("&Launch M580"), my_mainWidget);
    M580_button * stopbutton        = new M580_button(tr("&Stop M580"), my_mainWidget);
    ValveUsecase* my_usecase        = new ValveUsecase();

    connect(diode_button, SIGNAL(clicked()), my_diode, SLOT(setPower()));
    connect(shbutton    , SIGNAL(clicked()), this    , SLOT(launchbash()));
    connect(stopbutton  , SIGNAL(clicked()), this    , SLOT(killM580()));

    my_Vlayout0->addLayout(my_Hlayout);
    my_Vlayout0->addWidget(my_usecase);

    my_Vlayout ->addWidget(my_diode);
    my_Vlayout ->addItem(horizontalSpacer);
    my_Vlayout ->addWidget(diode_button);
    my_Vlayout2->addWidget(shbutton);
    my_Vlayout2->addWidget(stopbutton);

    my_Hlayout->addLayout(my_Vlayout);
    my_Hlayout->addWidget(my_sender);
    my_Hlayout->addLayout(my_Vlayout2);

    my_mainWidget->setLayout(my_Vlayout0);

}

void MainWindow::launchbash()
{
    my_sender->WriteText("<em>M580 launching...</em> \r");
    my_sender->setroot(1);
    process = new QProcess(this);
    process->start("/bin/sh", QStringList()<< path);
    id = process->processId() + 1;                          //Je ne sais pas pourquoi j'ai un décalage de 1
}

void MainWindow::killM580()
{
    if(my_sender->getroot())
    {
        my_sender->WriteText("<em>M580 closing...</em> \r");
        my_sender->setroot(0);

        QString ID = QString::number(id);                       //conversion Qstring
        QString  cmd = "kill " + ID;
        qDebug() << "cmd :" << cmd;
        QByteArray ba = cmd.toLocal8Bit();                      //conversion char*
        system( ba.data() );
        my_sender->WriteText("<em>Process end succeeds</em> \r");
    }

}


MainWindow::~MainWindow()
{
    killM580();
    qDebug() << "Exit!" ;
}


