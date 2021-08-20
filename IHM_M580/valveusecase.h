#ifndef VALVEUSECASE_H
#define VALVEUSECASE_H

#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QList>
#include <QPushButton>
#include <QDebug>
#include "m580.h"

class ValveUsecase : public QWidget
{
    Q_OBJECT

public:
    explicit ValveUsecase(QWidget *parent = nullptr);

private:
    QList<QLabel*> listLabel;
    QList<QPixmap*> listPixmap;
    QLabel* img;
    M580 *machine;


signals:

public slots:
    void change();

};

#endif // VALVEUSECASE_H
