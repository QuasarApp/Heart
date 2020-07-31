#include "hostaddress.h"
namespace NP {

HostAddress::HostAddress() {

}

unsigned short HostAddress::port() const {
    return _port;
}

void HostAddress::setPort(unsigned short port) {
    _port = port;
}
}
