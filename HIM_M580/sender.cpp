#include "sender.h"
#include <QDebug>

Sender::Sender()
{
    setupUi(this);

/******************************* UDP *****************************************/
    socket = new QUdpSocket(this);
    connect(socket, SIGNAL(readyRead()), this, SLOT(donneesRecues()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(erreurSocket(QAbstractSocket::SocketError)));
    tailleMessage = 0;
}

void Sender::on_btnbind_clicked()
{
    textEdit->append(tr("<em>Tentative de bind en cours...</em>"));

    socket->bind(QHostAddress(IPadress->text()), portsource->value());

    textEdit->append(tr("<em>Ok</em>"));

}

void Sender::on_command_returnPressed()
{
    QByteArray Data;
    message = command->text();
   Data.append(message.toStdString().c_str());

    socket->writeDatagram(Data, QHostAddress(IPadress->text()), portdestination->value());
    message = tr("<strong>") + "User" +tr("</strong> : ") + message;
    textEdit->append(message);

    command->clear(); // On vide la zone d'écriture du message
    command->setFocus(); // Et on remet le curseur à l'intérieur
}

void Sender::donneesRecues()
{
    // when data comes in
    QByteArray buffer;
    QByteArray display;
    buffer.resize(socket->pendingDatagramSize());

    QHostAddress sender;
    quint16 senderPort;

    socket->readDatagram(buffer.data(), buffer.size(),
                         &sender, &senderPort);
    portdestination->setValue(senderPort);

//    qDebug() << "Message from: " << sender.toString();
//    qDebug() << "Message port: " << senderPort;
//    qDebug() << "Message: " << buffer;
//    qDebug() << "Message size: " << buffer.size() << "\n";
//    qDebug() << buffer[buffer.size()-1];

textEdit->append(tr("<strong>") + sender.toString() +":"+ QString::number(senderPort) +tr("</strong> : ") + buffer);

//    if( buffer[buffer.size()-1] == ''){
//        textEdit->append(tr("<strong>") + sender.toString() +":"+ QString::number(senderPort) +tr("</strong> : ") + display);
//        display = "";
//    }
//    else
//        display += buffer;

}

void Sender::erreurSocket(QAbstractSocket::SocketError erreur)
{
    switch(erreur) // On affiche un message différent selon l'erreur qu'on nous indique
    {
        case QAbstractSocket::HostNotFoundError:
            textEdit->append(tr("<em>ERREUR : le serveur n'a pas pu être trouvé. Vérifiez l'IP et le port.</em>"));
            break;
        default:
            textEdit->append(tr("<em>ERREUR : ") + socket->errorString() + tr("</em>"));
    }
}

Sender::~Sender(){}
