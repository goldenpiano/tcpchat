#include "client.h"
#include "ui_client.h"

#include <QMessageBox>

Client::Client(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Client)
{
    ui->setupUi(this);
    m_pSocket = new QTcpSocket();

    connect(m_pSocket, SIGNAL(connected()), SLOT(slotConnected()));
    connect(m_pSocket, SIGNAL(disconnected()), SLOT(slotDisconnected()));
    connect(m_pSocket, SIGNAL(readyRead()), SLOT(slotReadyRead()));
    connect(m_pSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this,      SLOT(slotError(QAbstractSocket::SocketError))
            );
}

Client::~Client()
{
    delete ui;
}

void Client::on_m_btnConnect_clicked()
{
    if (m_pSocket->state() == QTcpSocket::ConnectedState) {
        m_pSocket->close();
    } else {
        QString m_ip = ui->m_ipEdit->text();
        uint16_t m_port = ui->m_portEdit->text().toInt();

        if(ui->m_ipEdit->text().isEmpty() || ui->m_portEdit->text().isEmpty())
        {
            QMessageBox::critical(0, "Client Error", "Connection fields not filled!");
            return;
        }
        m_pSocket->connectToHost(m_ip, m_port);
    }
}

void Client::slotReadyRead()
{
    QDataStream in(m_pSocket);
    if (m_pSocket->bytesAvailable() >= sizeof(Packet))
    {
        Packet packet;
        in >> packet;
        appendChatMessage(packet.nickname(), packet.message());

        if (ui->ddosBox->isChecked())
            sendToServer(packet );
    }
}

void Client::slotError(QAbstractSocket::SocketError err)
{
    QString strError =
            "Error: " + (err == QAbstractSocket::HostNotFoundError ?
                            "The host was not found." :
                         err == QAbstractSocket::RemoteHostClosedError ?
                            "The remote host is closed." :
                         err == QAbstractSocket::ConnectionRefusedError ?
                            "The connection was refused." :
                            QString(m_pSocket->errorString())
                         );
    appendInfoMessage(strError);
}

void Client::sendToServer(const QString& str, const QString &nick)
{
    Packet packet(str, nick);
    QByteArray array;

    QDataStream stream(&array, QIODevice::WriteOnly);
    stream << packet;

    m_pSocket->write(array);
    ui->m_ptxtInput->setText("");
}

void Client::sendToServer(const Packet &packet)
{
    QByteArray array;
    QDataStream stream(&array, QIODevice::WriteOnly);

    stream << packet;
    m_pSocket->write(array);
}

void Client::slotConnected()
{
    ui->groupBox->setTitle("Connected");
    ui->m_btnConnect->setText("Disconnect");

    appendInfoMessage("Status: Connected to server successfully");

    QString message = "Hello, Server!";
    QString nickname = ui->nickEdit->text();

    sendToServer(message, nickname);
    appendChatMessage(nickname, message);
}

void Client::slotDisconnected()
{
    ui->groupBox->setTitle("Disconnected");
    ui->m_btnConnect->setText("Connect");
    appendInfoMessage("Status: Disconnected from server!");
}

void Client::on_m_btnSend_clicked()
{
    if(!ui->m_ptxtInput->text().isEmpty())
    {
        if(m_pSocket->state() != QTcpSocket::ConnectedState) {
            ui->m_ptxtInfo->append("Error: Not connected to server!");
        }
        else {
            QString message = ui->m_ptxtInput->text();
            QString nickname = ui->nickEdit->text();

            sendToServer(message, nickname);
            appendChatMessage(nickname, message);
        }
    }
}

void Client::appendChatMessage(const QString &sender, const QString &message)
{
    ui->m_ptxtInfo->append( QString("%1 [%2] > %3").arg(QTime::currentTime().toString())
                                                   .arg(sender)
                                                   .arg(message)
                            );
}

void Client::appendInfoMessage(const QString &info)
{
    ui->m_ptxtInfo->append( QString("%1 %2").arg(QTime::currentTime().toString())
                                              .arg(info)
                            );
}
