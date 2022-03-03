/*
 * Copyright (C) 2018-2022 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "websocket.h"

#include <QDataStream>
#include <QSharedPointer>
namespace QH {
namespace PKG {

WebSocket::WebSocket(): AbstractData(){

}

WebSocket::WebSocket(const Package &package):
    WebSocket() {
    fromBytes(package.data);

}

QDataStream &WebSocket::fromStream(QDataStream &stream) {
    AbstractData::fromStream(stream);
    stream >> _request;
    stream >> _subscribeId;

    return stream;
}

QDataStream &WebSocket::toStream(QDataStream &stream) const {
    AbstractData::toStream(stream);
    stream << _request;
    stream << _subscribeId;

    return stream;
}

unsigned char WebSocket::getRequestCmd() const {
    return static_cast<unsigned char>(_request);
}

unsigned int WebSocket::subscribeId() const {
    return _subscribeId;
}

void WebSocket::setSubscribeId(unsigned int address) {
    _subscribeId = address;
}

void WebSocket::setRequestCommnad(const WebSocketRequest &requset) {
    _request = requset;
}

bool WebSocket::isValid() const {
    return _request > WebSocketRequest::Invalied
            && AbstractData::isValid();
}

}
}
