#include "btn.h"
#include <QDebug>
#include <QMessageBox>

BTN::BTN(QString text, QWidget *parent)
    : QPushButton(parent), m_text(text)
{
    setText(m_text);
    setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
}

BTN::~BTN()
{
    qDebug() << "Destruction !" ;
}

//void BTN::ActionButton(){
//    QMessageBox::information(this, "Toto", "Ceci est un message");
//}
