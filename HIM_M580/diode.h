#ifndef DIODE_H
#define DIODE_H

#include <QObject>
#include <QWidget>
#include <QString>


class Diode : public QWidget
{
    Q_OBJECT


public:
    explicit Diode(QWidget *parent = nullptr);
    ~Diode();

public slots:
    void setOn();
    void setOff();
    void setPower();

signals:

private:
    bool m_isClicked;
    bool power;


protected:
    void paintEvent(QPaintEvent* event);

};

#endif // DIODE_H
