#include "websocket.h"

#include <QDataStream>
#include <QSharedPointer>
namespace NetworkProtocol {

WebSocket::WebSocket(): DBObject(""){
    generateId<decltype (this)>();

}

WebSocket::WebSocket(const Package &package):
    DBObject("", package) {
    generateId<decltype (this)>();

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
            && AbstractData::isValid();
}

QSharedPointer<DBObject> WebSocket::factory() {return {nullptr};}

bool WebSocket::select(QSqlQuery &) { return false; }

bool WebSocket::save(QSqlQuery &) { return false; }

bool WebSocket::remove(QSqlQuery &) { return false; }

}
