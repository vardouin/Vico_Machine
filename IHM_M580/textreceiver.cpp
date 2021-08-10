#include "textreceiver.h"

TextReceiver::TextReceiver(QWidget *parent) : QWidget(parent)
{
    editor = new QTextEdit();
    editor->setPlainText("I'm the receiver, do not write here");
    editor->setStyleSheet("background-color: rgb(229, 232, 232);");

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(editor);

    setWindowTitle(tr("Receiver"));
}

void TextReceiver::setMessage(const Message &message)
{
    thisMessage = message;
    editor->setPlainText(thisMessage.body());
}
