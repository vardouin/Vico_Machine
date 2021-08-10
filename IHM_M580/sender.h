#ifndef SENDER_H
#define SENDER_H

#include <QWidget>
#include <QtNetwork>
#include <QFont>
#include <QDebug>
#include "m580_button.h"
#include "ui_sender.h"

class Sender : public QWidget, private Ui::Sender
{
    Q_OBJECT

public:
    Sender();
    ~Sender();
    void WriteText(QString text);
    void setroot(bool c);
    bool getroot();

private slots:
    void on_btnbind_clicked();
    void on_command_returnPressed();
    void donneesRecues();

private:
    QUdpSocket *socket; // Représente le serveur

    quint16 tailleMessage;
    QString message;
    QString display;
    bool root =0;

};

#endif // SENDER_H
