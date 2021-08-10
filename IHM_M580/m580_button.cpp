#include "m580_button.h"

M580_button::M580_button(QString text, QWidget *parent) : QPushButton(parent), m_text(text)
{
    setText(m_text);
    setStyleSheet("background-color: rgb(237, 187, 153);"
                  "color: rgb(255, 255, 255);");
}

void M580_button::setName(QString text)
{
    m_text = text;
}

QString M580_button::getName()
{
    return m_text;
}

M580_button::~M580_button()
{

}
