#include "server.h"
#include "ui_server.h"

Server::Server(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Server)
{
    ui->setupUi(this);
    m_server = new QTcpServer(this);

    connect(m_server, SIGNAL(newConnection()),
            this,     SLOT(slotNewConnection())
            );
}

Server::~Server()
{
    closeConnections();
    delete ui;
}

void Server::closeConnections()
{
    foreach (QTcpSocket* sock, m_clientSockList)
    {
        if(sock->state() == QTcpSocket::ConnectedState)
            sock->close();
    }
    m_server->close();
}

void Server::on_m_btnStart_clicked()
{
    if(!ui->lineEdit->text().isEmpty())
    {
        QHostAddress iface = QHostAddress::Any; // any = 0.0.0.0 = all interfaces :)
        m_port = ui->lineEdit->text().toInt();

        if (!m_server->isListening())
        {
            bool ok = m_server->listen(iface, m_port);
            if (ok) {
                ui->groupBox->setTitle("Server started");
                appendInfoMessage("Info: Server started");
                saveLog("Server started", iface.toString(), m_port);

                ui->m_btnStart->setText("Stop");
                ui->m_btnStart->setChecked(true);
            }
            else {
                appendInfoMessage("Error: Server start failed");
                saveLog("Server start failed", iface.toString(), m_port);
            }
        }
        else {
            closeConnections();

            ui->groupBox->setTitle("Server not started");
            appendInfoMessage("Info: Server stopped");
            saveLog("Server stopped", iface.toString(), m_port);

            ui->m_btnStart->setText("Start");
            ui->m_btnStart->setChecked(false);
        }
    }
}

void Server::slotNewConnection()
{
    QTcpSocket *newClient = m_server->nextPendingConnection();

    connect(newClient, SIGNAL(disconnected()),
            this,      SLOT(slotClientDisconnected())
            );
    connect(newClient, SIGNAL(readyRead()),
            this,      SLOT(slotReadClient())
            );

    m_clientSockList.push_back(newClient);
    sendToClient("Hello, Client!", newClient);
}

void Server::slotClientDisconnected()
{
    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());
    QString nickName = m_nickNameList.value(client, "");

    m_clientSockList.removeOne(client);
    m_nickNameList.remove(client);

    client->deleteLater();

    QString info = "Client [" + nickName + "] disconnected";
    appendInfoMessage(info);
    saveLog(info, client->peerAddress().toString(), client->peerPort());
}

void Server::slotReadClient()
{
    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());

    if(client->state()== QTcpSocket::ConnectedState)
    {
        QDataStream stream(client);
        if (client->bytesAvailable() >= sizeof(Packet))
        {
            Packet packet;
            stream >> packet;

            if (!m_nickNameList.contains(client)) {
                QString info = "Client [" + packet.nickname() + "] joined chat";
                appendInfoMessage(info);
                saveLog(info, client->peerAddress().toString(), client->peerPort());

                m_nickNameList.insert(client, packet.nickname());
            }

            appendChatMessage(packet.nickname(), packet.message());
            resend(packet, client);
        }
    }
    else {
        appendInfoMessage("Error: Unable to read from client which is not connected");
    }
}

void Server::sendToClient(const QString &message, QTcpSocket *dest)
{
    QByteArray array;
    QDataStream stream(&array, QIODevice::WriteOnly);

    Packet packet(message, "Server");
    stream << packet;
    dest->write(array);

    QString info;
    if (m_nickNameList.contains(dest)) {
        info = QString("Server -> [%1] > %2")
                       .arg(m_nickNameList.value(dest, ""))
                       .arg(message);
    }
    else {
        info = QString("Server > %1").arg(message);
    }

    saveLog(info, dest->peerAddress().toString(), dest->peerPort());
    appendInfoMessage(info);
}

void Server::sendToClient(const Packet &packet, QTcpSocket *dest)
{
    QByteArray array;
    QDataStream stream(&array, QIODevice::WriteOnly);

    stream << packet;
    dest->write(array);

    QString info = QString("Resend [%1] -> [%2] > %3")
                           .arg(packet.nickname())
                           .arg(m_nickNameList.value(dest, ""))
                           .arg(packet.message());

    saveLog(info, dest->peerAddress().toString(), dest->peerPort());
    appendInfoMessage(info);
}

void Server::on_m_btnSend_clicked()
{
    QString message = ui->lineEdit_2->text();
    foreach (QTcpSocket *sock, m_clientSockList) {
        if(sock->state() == QTcpSocket::ConnectedState) {
            sendToClient(message, sock);
        }
    }
}

void Server::saveLog(const QString &message, const QString &host, int port)
{
    QFile logfile("server.log");

    if(logfile.open(QIODevice::Append)) {
        QTextStream stream(&logfile);

        stream << QString("%1 | %2:%3 | %4\n")
                          .arg(QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm:ss"))
                          .arg(host).arg(port)
                          .arg(message);
        logfile.close();

        if (stream.status() != QTextStream::Ok) {
            qDebug() << "Write Error!";
        }
    }
}

void Server::resend(const Packet &packet, QTcpSocket *src)
{
    foreach (QTcpSocket *sock, m_clientSockList)
    {
        if(sock == src) { continue; }
        if(sock->state() == QTcpSocket::ConnectedState) {
            sendToClient(packet, sock);
        }
    }
}

void Server::appendChatMessage(const QString &sender, const QString &message)
{
    ui->m_ptxt->append( QString("%1 [%2] > %3").arg(QTime::currentTime().toString())
                                               .arg(sender)
                                               .arg(message)
                       );
}

void Server::appendInfoMessage(const QString &info)
{
    ui->m_ptxt->append( QString("%1 %2").arg(QTime::currentTime().toString())
                                         .arg(info)
                       );
}
