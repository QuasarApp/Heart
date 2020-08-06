#include "hostaddress.h"
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
    _port = port;
}
}
