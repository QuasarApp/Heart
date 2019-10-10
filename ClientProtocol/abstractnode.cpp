#include "abstractdata.h"
#include "abstractnode.h"
#include <quasarapp.h>

namespace ClientProtocol {

AbstractNode::AbstractNode(bool ssl, QObject *ptr):
    QTcpServer(ptr) {
    useSSL = ssl;
}

bool AbstractNode::run(const QString &addres, unsigned short port) {
    if (!listen(QHostAddress(addres), port)) {
        QuasarAppUtils::Params::verboseLog("Run fail " + this->errorString(),
                                           QuasarAppUtils::Error);
        return false;
    }

    return true;
}

void AbstractNode::stop() {
    close();

    for (auto &&i : _connections) {
        i.info.disconnect();
    }
}

AbstractNodeInfo *AbstractNode::getInfoPtr(quint32 id) {
    if (!_connections.contains(id)) {
        return nullptr;
    }

    return &_connections[id].info;
}

AbstractNodeInfo AbstractNode::getInfo(quint32 id) const{
    return _connections.value(id).info;
}

void AbstractNode::ban(quint32 target) {

    auto info = getInfoPtr(target);

    if (!info)
        _connections[target] = NodeInfoData{};

    _connections[target].info.ban();
}

void AbstractNode::unBan(quint32 target) {
    if (!_connections.contains(target)) {
        return;
    }

    _connections[target].info.unBan();
}

unsigned short AbstractNode::port() const {
    return serverPort();
}

QHostAddress AbstractNode::address() const {
    return serverAddress();
}

AbstractNode::~AbstractNode() {
    stop();
}

bool AbstractNode::registerSocket(QAbstractSocket *socket) {

    auto info = AbstractNodeInfo(socket);
    _connections[info.id()] = {info, {}};

    connect(socket, &QAbstractSocket::readyRead, this, &AbstractNode::avelableBytes);
    connect(socket, &QAbstractSocket::disconnected, this, &AbstractNode::handleDisconnected);

    return true;
}

bool AbstractNode::parsePackage(const BasePackage &pkg, QAbstractSocket *sender) {

    if (!pkg.isValid()) {
        QuasarAppUtils::Params::verboseLog("incomming package is not valid!",
                                           QuasarAppUtils::Error);
        changeTrust(qHash(sender->peerAddress()), CRITICAL_ERROOR);
        return false;
    }

    if (!sender->isValid()) {
        QuasarAppUtils::Params::verboseLog("sender socket is not valid!",
                                           QuasarAppUtils::Error);
        changeTrust(qHash(sender->peerAddress()), LOGICK_ERROOR);
        return false;
    }

    emit incomingReques(pkg, qHash(sender->peerAddress()));

    return true;
}

bool AbstractNode::sendPackage(const BasePackage &pkg, QAbstractSocket *target) {
    if (!pkg.isValid()) {
        return false;
    }

    if (!target || !target->isValid()) {
        QuasarAppUtils::Params::verboseLog("destination server not valid!",
                                           QuasarAppUtils::Error);
        return false;
    }

    if (!target->waitForConnected()) {
        QuasarAppUtils::Params::verboseLog("no connected to server! " + target->errorString(),
                                           QuasarAppUtils::Error);
        return false;
    }

    auto bytes = pkg.toBytes();
    bool sendet = bytes.size() == target->write(bytes);

    return sendet;
}

bool AbstractNode::sendResponse(const AbstractData &resp, quint32 id, const BaseHeader *req) {
    auto client = getInfoPtr(id);

    if (!client) {
        QuasarAppUtils::Params::verboseLog("Response not sent because client == null",
                                           QuasarAppUtils::Error);
        return false;
    }

    BasePackage pkg;
    if (!resp.toPackage(pkg, req->command)) {
        QuasarAppUtils::Params::verboseLog("Response not sent because dont create package from object",
                                           QuasarAppUtils::Error);
        return false;
    }

    if (!sendPackage(pkg, client->sct())) {
        QuasarAppUtils::Params::verboseLog("Response not sent!",
                                           QuasarAppUtils::Error);
        return false;
    }

    return true;
}

// TO DO hasPendingConnections is no implementing
QString AbstractNode::getWorkState() const {
    if (isListening()) {
        if (hasPendingConnections())
            return "overload";
        else {
            return "Work";
        }
    }

    return "Not running";
}

QString AbstractNode::connectionState() const {
    return QString("%0 / %1").arg(connectionsCount()).arg(maxPendingConnections());
}

QStringList AbstractNode::baned() const {
    QStringList list = {};
    for (auto i = _connections.begin(); i != _connections.end(); ++i) {
        if (i.value()->isBaned()) {
            list.push_back(QHostAddress(i.key()).toString());
        }
    }

    return list;
}

int AbstractNode::connectionsCount() const {
    int count = 0;
    for (auto i : _connections) {
        if (i->getSct()) {
            if (!i->getSct()->isValid()) {
                QuasarAppUtils::Params::verboseLog("connection count, findet not valid socket",
                                                   QuasarAppUtils::Warning);
            }

            count++;
        }
    }
    return count;
}

bool AbstractNode::isBaned(QAbstractSocket *socket) const {
    auto ptr = _connections.value(adr->peerAddress().toIPv4Address());

    if (!ptr) {
        return false;
    }

    return ptr->isBaned();
}

void AbstractNode::incomingConnection(qintptr handle) {

}

bool AbstractNode::changeTrust(quint32 id, int diff) {
    auto ptr = getInfoPtr(id);
    if (!ptr) {
        return false;
    }

    auto objTrust = ptr->trust();

    if (objTrust >= static_cast<int>(TrustNode::Undefined)) {
        return false;
    }

    if (objTrust <= static_cast<int>(TrustNode::Baned)) {
        return false;
    }

    ptr->setTrust(objTrust + diff);
    return true;
}


void AbstractNode::avelableBytes() {
    auto client = dynamic_cast<QTcpSocket*>(sender());

    if (!client) {
        return;
    }

    auto array = client->readAll();

    if (_downloadPackage.hdr.isValid()) {
        _downloadPackage.data.append(array);

    } else {
        _downloadPackage.reset();

        memcpy(&_downloadPackage.hdr,
               array.data(), sizeof(BaseHeader));

        _downloadPackage.data.append(array.mid(sizeof(BaseHeader)));
    }

    if (_downloadPackage.isValid()) {
        parsePackage(_downloadPackage, client);
    }

    if (_downloadPackage.data.size() >= _downloadPackage.hdr.size) {
        _downloadPackage.reset();
    }
}

void AbstractNode::handleDisconnected() {
    auto _sender = dynamic_cast<QTcpSocket*>(sender());

    if (_sender) {
        // log error

        unsigned int address = _sender->peerAddress().toIPv4Address();
        auto ptr = _connections.value(address);
        if (ptr) {
            ptr->disconnect();
        } else {
            QuasarAppUtils::Params::verboseLog("system error in void Server::handleDisconected()"
                                               " address not valid",
                                               QuasarAppUtils::Error);
        }
        return;
    }

    QuasarAppUtils::Params::verboseLog("system error in void Server::handleDisconected()"
                                       "dynamic_cast fail!",
                                       QuasarAppUtils::Error);
}
}
