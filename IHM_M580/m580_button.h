#ifndef M580_BUTTON_H
#define M580_BUTTON_H

#include <QPushButton>
#include <QObject>
#include <QWidget>

class M580_button : public QPushButton
{
public:
    M580_button(QString text, QWidget *parent = nullptr);
    QString getName();
    void setName(QString text);
    ~M580_button();

private:
    QString m_text;
};

#endif // M580_BUTTON_H
