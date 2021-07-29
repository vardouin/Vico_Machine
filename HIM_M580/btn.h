#ifndef BTN_H
#define BTN_H

#include <QPushButton>
#include <QObject>

class BTN : public QPushButton
{

    Q_OBJECT

public slots:
    //void ActionButton();

public:
    BTN(QString text, QWidget *parent = nullptr);

    QString getName() {return m_text;}
    void setName(QString text){m_text = text;}

    ~BTN();

private :
    QString m_text;
};

#endif // BTN_H
