#include "diode.h"
#include <QPainter>
#include <QDebug>


Diode::Diode(QWidget *parent) : QWidget(parent)
{
    //qDebug() << "Création de la diode!" ;
    m_isClicked = false;
    power = true;

}

void Diode::paintEvent(QPaintEvent *event)
{
    QPainter p(this);

    p.setPen(Qt::black);

    p.setBrush(m_isClicked ? Qt::darkGreen : Qt::red );

    p.drawEllipse((this->width())/2,(this->height())/2, 40,40);
    updateGeometry();
}

void Diode::setOn()
{
    m_isClicked = true;
    power =false;
    repaint();
}

void Diode::setOff()
{
    m_isClicked = false;
    power =true;
    repaint();
}

void Diode::setPower()
{
  if(power) setOn();
  else setOff();
}

Diode::~Diode()
{
        //qDebug() << "Destruction de la diode!" ;
}
