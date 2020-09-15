/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "request.h"
namespace QH {

Request::Request() {

}

unsigned char Request::getRequestCmd() const {
    return requestCmd;
}

void Request::setRequestCmd(unsigned char value) {
    requestCmd = value;
}

}
