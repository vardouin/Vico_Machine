#ifndef TEXTSENDER_H
#define TEXTSENDER_H

#include <QObject>
#include <QWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "message.h"

class TextSender : public QWidget
{
    Q_OBJECT
public:
    explicit TextSender(QWidget *parent = nullptr);

signals:
    void messageSent(const Message &message);

public slots:

private slots:
    void sendMessage();

private:
    Message thisMessage;
    QTextEdit *editor;
};

#endif // TEXTSENDER_H
