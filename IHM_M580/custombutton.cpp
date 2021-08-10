#include "custombutton.h"
#include <QDebug>

CustomButton::CustomButton(QString text, QWidget *parent) : QPushButton(parent), m_text(text)
{
    qDebug() << "Création du bouton!" ;
    setText(m_text);
    setToolTip("Clic to power On/Off");
    setStyleSheet("background-color: rgb(237, 187, 153);"
                  "border-width: 10px;"
                  "border-radius: 40px;"
                  "border-color: beige;"
                  "color: rgb(255, 255, 255);");

    //setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
}


void CustomButton::mousePressEvent(QMouseEvent *e)
{
    emit PressMyBtn();
    setStyleSheet("background-color: rgb(255, 255, 255);"
                  "border-width: 10px;"
                  "border-radius: 40px;"
                  "border-color: beige;"
                  "color: rgb(0, 0, 0);");
}
void CustomButton::mouseReleaseEvent(QMouseEvent *e)
{
    setStyleSheet("background-color: rgb(237, 187, 153);"
                  "border-width: 10px;"
                  "border-radius: 40px;"
                  "border-color: beige;"
                  "color: rgb(255, 255, 255);");
}

CustomButton::~CustomButton()
{
    qDebug() << "Destruction du bouton!" ;
}
