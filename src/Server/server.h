#pragma once

#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QTcpServer>
#include <QWidget>
#include <QtGui>
#include <QFile>
#include <QDataStream>

#include "../Structs/packet.h"

class QTcpServer;
class QTextEdit;
class QTcpSocket;

namespace Ui {
class Server;
}

class Server : public QWidget
{
    Q_OBJECT

public:
    explicit Server(QWidget *parent = 0);
    ~Server();

private:
    void closeConnections();

    void sendToClient(const QString &message, QTcpSocket *dest);
    void sendToClient(const Packet &packet, QTcpSocket *dest);
    void resend(const Packet &packet, QTcpSocket *src);

    void saveLog(const QString &message, const QString &host, int port);

    void appendInfoMessage(const QString &info);
    void appendChatMessage(const QString &sender, const QString &message);

public slots:
    void slotNewConnection();
    void slotReadClient();
    void slotClientDisconnected();

private slots:
    void on_m_btnStart_clicked();
    void on_m_btnSend_clicked();

private:
    Ui::Server *ui;

private:
    QTcpServer* m_server;
    QList<QTcpSocket*> m_clientSockList;
    QMap<QTcpSocket*,QString> m_nickNameList;
    int m_port;
};
