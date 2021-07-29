#include "sender.h"
#include <QDebug>

Sender::Sender()
{
    setupUi(this);
/******************************* TCP ******************************************/
//    socket = new QTcpSocket(this);
//    connect(socket, SIGNAL(readyRead()), this, SLOT(donneesRecues()));
//    connect(socket, SIGNAL(connected()), this, SLOT(connecte()));
//    connect(socket, SIGNAL(disconnected()), this, SLOT(deconnecte()));
//    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(erreurSocket(QAbstractSocket::SocketError)));

/******************************* UDP *****************************************/
    socket = new QUdpSocket(this);
    connect(socket, SIGNAL(readyRead()), this, SLOT(donneesRecues()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(erreurSocket(QAbstractSocket::SocketError)));
    tailleMessage = 0;
}

Sender::~Sender()
{

}

//void Sender::on_btnconnexion_clicked()
//{
//    textEdit->append(tr("<em>Tentative de connexion en cours...</em>"));
//    btnconnexion->setEnabled(false); //on n'autorise pas d'autres connexion quand un est en cours
//    socket->abort();                 // On désactive les connexions précédentes s'il y en a
//    socket->connectToHost(IPadress->text(), portnumber->value());
//}

//void Sender::connecte()
//{
//    textEdit->append(tr("<em>Connexion réussie !</em>"));
//    btnconnexion->setEnabled(true);
//}

//void Sender::deconnecte()
//{
//    textEdit->append(tr("<em>Déconnecté du serveur</em>"));
//}


void Sender::on_btnbind_clicked()
{
    textEdit->append(tr("<em>Tentative de bind en cours...</em>"));

    socket->bind(QHostAddress(IPadress->text()), portsource->value());

    textEdit->append(tr("<em>Ok</em>"));

}

void Sender::on_command_returnPressed()
{
    /******************************* TCP ******************************************/

//    QByteArray paquet;
//    QDataStream out(&paquet, QIODevice::WriteOnly);

//    // On prépare le paquet à envoyer
//    QString messageAEnvoyer = tr("<strong>") + tr("User") +tr("</strong> : ") + command->text();

//    out << (quint16) 0;
//    out << messageAEnvoyer;
//    out.device()->seek(0);
//    out << (quint16) (paquet.size() - sizeof(quint16));

//    socket->write(paquet); // On envoie le paquet

    /******************************* UDP *****************************************/

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
    /******************************* TCP ******************************************/

//    /* On essaie de récupérer la taille du message
//    Une fois qu'on l'a, on attend d'avoir reçu le message entier (en se basant sur la taille annoncée tailleMessage)
//    */
//    QDataStream in(socket);

//    if (tailleMessage == 0)
//    {
//        if (socket->bytesAvailable() < (int)sizeof(quint16))
//             return;

//        in >> tailleMessage;
//    }

//    if (socket->bytesAvailable() < tailleMessage)
//        return;


//    // Si on arrive jusqu'à cette ligne, on peut récupérer le message entier
//    QString messageRecu;
//    in >> messageRecu;

//    // On affiche le message sur la zone de Chat
//    textEdit->append(messageRecu);

//    // On remet la taille du message à 0 pour pouvoir recevoir de futurs messages
//    tailleMessage = 0;

    /******************************* UDP *****************************************/

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
        case QAbstractSocket::ConnectionRefusedError:
            textEdit->append(tr("<em>ERREUR : le serveur a refusé la connexion. Vérifiez si le programme \"serveur\" a bien été lancé. Vérifiez aussi l'IP et le port.</em>"));
            break;
        case QAbstractSocket::RemoteHostClosedError:
            textEdit->append(tr("<em>ERREUR : le serveur a coupé la connexion.</em>"));
            break;
        default:
            textEdit->append(tr("<em>ERREUR : ") + socket->errorString() + tr("</em>"));
    }

    //btnconnexion->setEnabled(true);
}
