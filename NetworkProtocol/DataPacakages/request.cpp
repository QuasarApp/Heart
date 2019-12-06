#include "request.h"
namespace NP {

Request::Request() {

}

unsigned char Request::getRequestCmd() const {
    return requestCmd;
}

void Request::setRequestCmd(unsigned char value) {
    requestCmd = value;
}

}
