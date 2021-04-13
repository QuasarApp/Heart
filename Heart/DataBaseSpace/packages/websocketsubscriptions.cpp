/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "websocketsubscriptions.h"

#include <QDataStream>

namespace QH {
namespace PKG {

WebSocketSubscriptions::WebSocketSubscriptions()
{
    

}

WebSocketSubscriptions::WebSocketSubscriptions(const QH::Package &package):
    WebSocketSubscriptions()
{
    fromBytes(package.data);

}

QDataStream &WebSocketSubscriptions::fromStream(QDataStream &stream) {
    AbstractData::fromStream(stream);

    return stream >> _subscribeIds;
}

QDataStream &WebSocketSubscriptions::toStream(QDataStream &stream) const {
    AbstractData::toStream(stream);

    return stream << _subscribeIds;
}

QSet<unsigned int> WebSocketSubscriptions::addresses() const {
    return _subscribeIds;
}

void WebSocketSubscriptions::setAddresses(const QSet<unsigned int> &addresses) {
    _subscribeIds = addresses;
}

}
}

