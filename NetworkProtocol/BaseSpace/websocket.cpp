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

WebSocket::WebSocket(): DBObject(""){
    INIT_COMMAND

}

WebSocket::WebSocket(const Package &package):
    WebSocket() {
    fromBytes(package.data);

}

QDataStream &WebSocket::fromStream(QDataStream &stream) {
    DBObject::fromStream(stream);
    return stream >> requestCmd;
}

QDataStream &WebSocket::toStream(QDataStream &stream) const {
    DBObject::toStream(stream);
    return stream << requestCmd;
}

bool WebSocket::isValid() const {
    return requestCmd > static_cast<int>(WebSocketRequest::Invalied)
            && AbstractData::isValid() && _from.isValid();
}

DBObject *WebSocket::factory() const {return nullptr;}

bool WebSocket::prepareSelectQuery(QSqlQuery &) const { return false; }

bool WebSocket::prepareSaveQuery(QSqlQuery &) const { return false; }

bool WebSocket::prepareRemoveQuery(QSqlQuery &)const { return false; }

}
