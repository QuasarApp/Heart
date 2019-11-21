#include "request.h"
namespace NetworkProtocol {

Request::Request() {

}

unsigned char Request::getRequestCmd() const {
    return requestCmd;
}

void Request::setRequestCmd(unsigned char value) {
    requestCmd = value;
}
}
