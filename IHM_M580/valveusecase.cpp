#include "valveusecase.h"

ValveUsecase::ValveUsecase(QWidget *parent) : QWidget(parent)
{
    setStyleSheet(
                "background-color: rgb(254, 245, 231);"
                "border-radius: 10px;");


    img = new QLabel ("Image");
    listPixmap << new QPixmap("/home/vardouin/Project/IHM_M580/Allume.png");
    listPixmap << new QPixmap("/home/vardouin/Project/IHM_M580/Eteint.png");
    machine = new M580();

    connect(machine, SIGNAL(clicked()), this, SLOT(change()));


    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(machine);
    layout->addWidget(img);
    img->setPixmap( *listPixmap[1] );
    //img->setScaledContents(1);

//    for(int i=0; i<listPixmap.size(); i++){
//       listLabel[i]->setPixmap( *listPixmap[i] );
//       listLabel[i]->setScaledContents(1);
//       layout->addWidget(listLabel[i]);
//    }

    setLayout(layout);
}

void ValveUsecase::change()
{
    if(!(machine->getpower())){
        qDebug() << "Ca s'allume";
        img->clear();
        img->setPixmap( *listPixmap[0] );
        //img->setScaledContents(1);
    }
    else{
        qDebug() << "Ca s'eteind";
        img->clear();
        img->setPixmap( *listPixmap[1] );
        //img->setScaledContents(1);
    }
}

