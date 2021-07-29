#ifndef SENDER_H
#define SENDER_H

#include <QWidget>
#include <QtNetwork>
#include "ui_sender.h"

class Sender : public QWidget, private Ui::Sender
{
    Q_OBJECT

public:
    Sender();
    ~Sender();

private slots:
//    void on_btnconnexion_clicked();
    void on_btnbind_clicked();
    void on_command_returnPressed();
    void donneesRecues();
//    void connecte();
//    void deconnecte();
    void erreurSocket(QAbstractSocket::SocketError erreur);

private:
    //QTcpSocket *socket; // Représente le serveur
    QUdpSocket *socket; // Représente le serveur

    quint16 tailleMessage;
    QString message;

};

#endif // SENDER_H
