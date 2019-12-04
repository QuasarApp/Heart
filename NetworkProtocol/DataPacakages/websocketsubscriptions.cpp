#include "websocketsubscriptions.h"

#include <QDataStream>

namespace NetworkProtocol {
WebSocketSubscriptions::WebSocketSubscriptions()
{
    generateId<decltype (this)>();

}

WebSocketSubscriptions::WebSocketSubscriptions(const NetworkProtocol::Package &package):
    AbstractData(package)
{
    generateId<decltype (this)>();

}

QDataStream &WebSocketSubscriptions::fromStream(QDataStream &stream) {
    AbstractData::fromStream(stream);
    return stream >> _addresses;
}

QDataStream &WebSocketSubscriptions::toStream(QDataStream &stream) const {
    AbstractData::toStream(stream);
    return stream << _addresses;
}

QSet<DbAddress> WebSocketSubscriptions::addresses() const
{
    return _addresses;
}

void WebSocketSubscriptions::setAddresses(const QSet<DbAddress> &addresses)
{
    _addresses = addresses;
}

}


