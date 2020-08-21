#include "hostaddress.h"
#include <QDataStream>
#include <quasarapp.h>
namespace NP {

HostAddress::HostAddress() {

}

HostAddress::HostAddress(const QHostAddress &other, int port):
    QHostAddress(other) {
    setPort(port);
}

HostAddress::HostAddress(const QString &other, int port):
    QHostAddress(other) {
    setPort(port);
}

HostAddress::HostAddress(const QHostAddress::SpecialAddress &other, int port):
    QHostAddress(other) {
    setPort(port);
}

HostAddress::HostAddress(const HostAddress &other):
    QHostAddress(*static_cast<const HostAddress*>(&other)) {

    setPort(other.port());
}

unsigned short HostAddress::port() const {
    return _port;
}

void HostAddress::setPort(unsigned short port) {
    debug_assert(port);

    _port = port;
}

QDataStream &operator >>(QDataStream &stream, HostAddress &address) {
    stream >> static_cast<QHostAddress&>(address);
    stream >> address._port;

    return stream;
}

QDataStream &operator <<(QDataStream &stream, const HostAddress &address) {

    stream << static_cast<const QHostAddress&>(address);
    stream << address._port;

    return stream;
}

uint qHash(const HostAddress &address) {
    return qHash(QString("%1:%2").arg(address.toString()).arg(address.port()));
}

}
