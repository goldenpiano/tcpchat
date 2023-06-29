#include <QCoreApplication>
#include <QDebug>
#include "packet.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Packet packet("Hello server", "Гульназ");

    QByteArray array;

    QDataStream in(&array, QIODevice::WriteOnly);
    in << packet;

    Packet res;

    QDataStream out(&array, QIODevice::ReadOnly);
    out >> res;

    qDebug() << "Result:" << res.message() << res.nickname();

    return 0;
}
