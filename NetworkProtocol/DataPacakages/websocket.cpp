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
            && AbstractData::isValid();
}

SP<DBObject> WebSocket::factory() {return {nullptr};}

bool WebSocket::select(QSqlQuery &) { return false; }

bool WebSocket::save(QSqlQuery &) { return false; }

bool WebSocket::remove(QSqlQuery &) { return false; }

}