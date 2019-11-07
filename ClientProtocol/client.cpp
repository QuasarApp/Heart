#include "client.h"
namespace ClientProtocol {

Client::Client(const QHostAddress &address, unsigned short port) {
    setHost(address, port);
}

void Client::connectClient() {
    connectToHost(_address, _port);
}

void Client::setHost(const QHostAddress &address, unsigned short port) {
    _address = address;
    _port = port;
}

int Client::status() const {
    return _status;
}

QString Client::lastMessage() const {
    return _lastMessage;
}

void Client::handleIncomingData(Package pkg, const QHostAddress&  sender) {
    auto cmd = QSharedPointer<UserDataRequest>::create(pkg);


}

void Client::setLastMessage(QString lastMessage) {
    if (_lastMessage == lastMessage)
        return;

    _lastMessage = lastMessage;
    emit lastMessageChanged(_lastMessage);
}

}
