#include "sender.h"

Sender::Sender()
{
    setupUi(this);

/******************************* UDP *****************************************/
    socket = new QUdpSocket(this);
    connect(socket, SIGNAL(readyRead()), this, SLOT(donneesRecues()));
    tailleMessage = 0;
}

void Sender::on_btnbind_clicked()
{
    textEdit->append(tr("<em>Bind attempt in progress...</em>"));

    socket->bind(QHostAddress(IPadress->text()), portsource->value());

    textEdit->append(tr("<em>Ok</em>"));

}

void Sender::on_command_returnPressed()
{
    QByteArray Data;
    message = command->text();
    message = message + "\n";
    Data.append( message.toStdString().c_str());
    socket->writeDatagram(Data, QHostAddress(IPadress->text()), portdestination->value());

    if(root == 0){
        message = tr("<strong>") + "User : " +tr("</strong> ") + message;
        textEdit->append(message);}
    textEdit->ensureCursorVisible();
    command->clear(); // On vide la zone d'écriture du message
    command->setFocus(); // Et on remet le curseur à l'intérieur
}

void Sender::donneesRecues()
{
    // when data comes in
    QByteArray buffer;
    buffer.resize(socket->pendingDatagramSize());

    QHostAddress sender;
    quint16 senderPort;

    socket->readDatagram(buffer.data(), buffer.size(),
                         &sender, &senderPort);
    portdestination->setValue(senderPort);

    //qDebug() << "Message: " << buffer;

    if( buffer == "\n"){
           textEdit->setFontWeight(QFont::Bold);
           textEdit->insertPlainText(tr("M580 : "));
           textEdit->setFontWeight(QFont::Normal);
    }
    else
        textEdit->insertPlainText(buffer);

    textEdit->ensureCursorVisible();
}

Sender::~Sender(){}

void Sender::WriteText(QString text)
{
    textEdit->append(text);
}

void Sender::setroot(bool c)
{
    root = c;
}

bool Sender::getroot()
{
    return root;
}
