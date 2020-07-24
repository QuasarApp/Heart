/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "websocketsubscriptions.h"

#include <QDataStream>

namespace NP {
WebSocketSubscriptions::WebSocketSubscriptions()
{
    INIT_COMMAND

}

WebSocketSubscriptions::WebSocketSubscriptions(const NP::Package &package):
    WebSocketSubscriptions()
{
    fromBytes(package.data);

}

QDataStream &WebSocketSubscriptions::fromStream(QDataStream &stream) {
    AbstractData::fromStream(stream);

    BaseId senderNode;
    stream >> senderNode;
    setSenderID(senderNode);

    return stream >> _addresses;
}

QDataStream &WebSocketSubscriptions::toStream(QDataStream &stream) const {
    AbstractData::toStream(stream);
    stream << senderID();

    return stream << _addresses;
}

QSet<DbAddress> WebSocketSubscriptions::addresses() const {
    return _addresses;
}

void WebSocketSubscriptions::setAddresses(const QSet<DbAddress> &addresses) {
    _addresses = addresses;
}

}


