/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "tcpsocket.h"
namespace QH {


TcpSocket::TcpSocket(QObject *parent):
    QTcpSocket(parent) {

}

TcpSocket::~TcpSocket() {
    int i  = 3;
}

void TcpSocket::handleDisckonnetFromHost() {
    disconnectFromHost();
}

}
