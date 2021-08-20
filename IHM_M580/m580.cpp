#include "m580.h"

M580::M580(QWidget *parent) : QPushButton(parent)

{
    setStyleSheet("background-color: rgb(254, 245, 231);");
    pix = new QPixmap("/home/vardouin/Project/IHM_M580/M580.png");
    setIcon(QIcon("/home/vardouin/Project/IHM_M580/M580.png"));
    setIconSize(pix->rect().size());

    connect(this, SIGNAL(clicked()), this, SLOT(setPower()));
    power = true;
}
M580::~M580(){}

bool M580::getpower()
{
    return power;
}

QLabel* M580::getlabel()
{
    return label;
}

/********************************SLOTS*****************************/

void M580::setPower()
{
    qDebug() << "ok";
    if(power) power = false;
    else power = true;
    qDebug()<< power;
}

