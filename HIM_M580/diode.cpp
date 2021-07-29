#include "diode.h"
#include <QPainter>
#include <QDebug>


Diode::Diode(QWidget *parent) : QWidget(parent)
{
    qDebug() << "Création de la diode!" ;
    m_isClicked = false;
    power = true;

}

void Diode::paintEvent(QPaintEvent *event)
{
    QPainter p(this);

    p.setPen(Qt::black);

    p.setBrush(m_isClicked ? Qt::darkGreen : Qt::red );
    p.drawEllipse(6, 6,this->width()-20,this->height()-20);
//    p.setPen(Qt::white);
//    p.drawText(this->width() * 0.5f - 40,this->height() *0.5f, "C'est mon Widget" );
    updateGeometry();
}

void Diode::setOn()
{
    //qDebug() << "Ca allume" ;
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
        qDebug() << "Destruction de la diode!" ;
}
