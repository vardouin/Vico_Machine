#ifndef CUSTOMBUTTON_H
#define CUSTOMBUTTON_H

#include <QPushButton>
#include <QObject>

class CustomButton : public QPushButton
{

    Q_OBJECT

public:
    CustomButton(QString text, QWidget *parent = nullptr);

    QString getName() {return m_text;}
    void setName(QString text){m_text = text;}

    ~CustomButton();


public slots:

protected:
        void mousePressEvent(QMouseEvent* e);
        void mouseReleaseEvent(QMouseEvent* e);
signals:
   void PressMyBtn();
    void ReleaseMyBtn();

private:
    QString m_text;

};

#endif // CUSTOMBUTTON_H
