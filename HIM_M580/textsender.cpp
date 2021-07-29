#include "textsender.h"

TextSender::TextSender(QWidget *parent) : QWidget(parent)
{
    editor = new QTextEdit();
    editor->setPlainText("I'm the sender, write your text here");
    editor->setStyleSheet("background-color: rgb(229, 232, 232);");
    QPushButton *sendButton = new QPushButton(tr("&Send message"));
    sendButton->setStyleSheet("background-color: rgb(237, 187, 153);"
                              "color: rgb(255, 255, 255);");

    connect(sendButton, SIGNAL(clicked()), this, SLOT(sendMessage()));

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(sendButton);
    buttonLayout->addStretch();

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(editor);
    layout->addLayout(buttonLayout);

    setWindowTitle(tr("Sender"));
}

void TextSender::sendMessage()
{
    thisMessage = Message(editor->toPlainText(), thisMessage.headers());
    emit messageSent(thisMessage);
}
