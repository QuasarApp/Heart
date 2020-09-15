/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "websocket.h"

#include <QDataStream>
#include <QSharedPointer>
namespace NP {

WebSocket::WebSocket(): AbstractData(){

}

WebSocket::WebSocket(const Package &package):
    WebSocket() {
    fromBytes(package.data);

}

QDataStream &WebSocket::fromStream(QDataStream &stream) {
    AbstractData::fromStream(stream);
    stream >> requestCmd;
    stream >> _address;

    return stream;
}

QDataStream &WebSocket::toStream(QDataStream &stream) const {
    AbstractData::toStream(stream);
    stream << requestCmd;
    stream << _address;

    return stream;
}

DbAddress WebSocket::address() const {
    return _address;
}

void WebSocket::setAddress(const DbAddress &address) {
    _address = address;
}

bool WebSocket::isValid() const {
    return requestCmd > static_cast<int>(WebSocketRequest::Invalied)
            && AbstractData::isValid();
}

}
