#ifndef TEXTRECEIVER_H
#define TEXTRECEIVER_H

#include <QObject>
#include <QWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "message.h"

class TextReceiver : public QWidget
{
    Q_OBJECT
public:
    explicit TextReceiver(QWidget *parent = nullptr);

signals:

public slots:
    void setMessage(const Message &message);

private slots:

private:
    Message thisMessage;
    QTextEdit *editor;
};

#endif // TEXTRECEIVER_H
