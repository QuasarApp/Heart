#include "websocket.h"

namespace ClientProtocol {

WebSocket::WebSocket() {
    _class = static_cast<quint8>(Command::WebSocket);
}

BaseNetworkObject *ClientProtocol::WebSocket::create() const {
    return new WebSocket();
}

NetworkClassSize WebSocket::classSize() const {
    return UpdatePlayerData::classSize() + getTypeSize(_data);

}

QDataStream &WebSocket::writeToStream(QDataStream &stream) const {
    UpdatePlayerData::writeToStream(stream);
    stream << _data;
    return stream;
}

QDataStream &WebSocket::readFromStream(QDataStream &stream) {
    UpdatePlayerData::readFromStream(stream);
    stream >> _data;
    return stream;
}

bool WebSocket::isValid() const {
    return static_cast<Command>(_data.cmd) !=  Command::Undefined &&
            UpdatePlayerData::isValid();

}

bool WebSocket::isSubscribe() const {
    return _data.subscribe;
}

Command WebSocket::getCommand() const {
    return static_cast<Command>(_data.cmd);
}

int WebSocket::getObjectId() const {
    return _data.objectId;
}

void WebSocket::setSubscribe(bool subscribe){
    _data.subscribe = subscribe;
}

void WebSocket::setCommand(Command cmd) {
    _data.cmd = static_cast<unsigned char>(cmd);
}

void WebSocket::setObjectId(int value) {
    _data.objectId = value;
}

QDataStream &operator<<(QDataStream &stream, WebSocketData data) {
    stream.writeRawData(reinterpret_cast<char*>(&data), sizeof (data));
    return stream;
}

QDataStream &operator>>(QDataStream &stream, WebSocketData &data) {
    stream.readRawData(reinterpret_cast<char*>(&data), sizeof (data));
    return stream;

}

}
