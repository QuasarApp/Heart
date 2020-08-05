#include "hostaddress.h"
namespace NP {

HostAddress::HostAddress() {

}

HostAddress::HostAddress(const QHostAddress &other, int port):
    QHostAddress(other) {
    setPort(port);
}

unsigned short HostAddress::port() const {
    return _port;
}

void HostAddress::setPort(unsigned short port) {
    _port = port;
}
}
