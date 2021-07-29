#include "message.h"

Message::Message()
{
}

Message::Message(const Message &other)
{
    m_body = other.m_body;
    m_headers = other.m_headers;
}

Message::~Message()
{
}

Message::Message(const QString &body, const QStringList &headers)
{
    m_body = body;
    m_headers = headers;
    qDebug() << "C'est :" << m_headers<<" le header \n Et :" <<m_body<<" le corps \n" ;
}

QString Message::body() const
{
    return m_body;
}

QStringList Message::headers() const
{
    return m_headers;
}
