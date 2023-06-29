#pragma once

#include <QWidget>
#include <QtNetwork/QTcpSocket>
#include <QtGui>

#include "../Structs/packet.h"

class QTextEdit;
class QLineEdit;

namespace Ui {
class Client;
}

class Client : public QWidget
{
    Q_OBJECT

public:
    explicit Client(QWidget *parent = 0);
    ~Client();

private slots:
    void slotConnected();
    void slotDisconnected();
    void slotReadyRead();
    void slotError(QAbstractSocket::SocketError);

    void on_m_btnConnect_clicked();
    void on_m_btnSend_clicked();

private:
    void sendToServer(const QString &str, const QString &nick);
    void sendToServer(const Packet &packet);

    void appendChatMessage(const QString &sender, const QString &message);
    void appendInfoMessage(const QString &info);

private:
    QTcpSocket* m_pSocket;
    Ui::Client *ui;
};
