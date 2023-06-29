#pragma once

#include <QDataStream>
#include <QString>

struct Packet
{
    Packet(){}
    Packet(const QString &message, const QString &nickname)
    {
        this->m_message = message;
        this->m_nickname = nickname;
    }

    friend QDataStream & operator<<(QDataStream &stream, const Packet &packet)
    {
        stream << packet.m_message << packet.m_nickname;
        return stream;
    }

    friend QDataStream & operator>>(QDataStream &stream, Packet &packet)
    {
        stream >> packet.m_message >> packet.m_nickname;
        return stream;
    }

    QString message() const  { return this->m_message;  }
    QString nickname() const { return this->m_nickname; }

private:
    QString m_message;
    QString m_nickname;
};
