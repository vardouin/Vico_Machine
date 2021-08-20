#ifndef M580_H
#define M580_H

#include <QObject>
#include <QPushButton>
#include <QWidget>
#include <QDebug>
#include <QLabel>
#include <QVBoxLayout>
#include <QIcon>

class M580 : public QPushButton
{
    Q_OBJECT

public:
    M580(QWidget *parent = nullptr);
    bool getpower();
    QLabel* getlabel();
    ~M580();

public slots:
    void setPower();

private:
    bool power;
    QLabel *label;
    QPixmap *pix;
};

#endif // M580_H
