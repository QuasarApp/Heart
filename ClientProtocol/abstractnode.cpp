#include "abstractdata.h"
#include "abstractnode.h"
#include <QSslSocket>
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

AbstractNodeInfo *AbstractNode::getInfoPtr(const QHostAddress &id) {
    return getInfoPtr(qHash(id));
}

AbstractNodeInfo AbstractNode::getInfo(const QHostAddress &id) const {
    return getInfo(qHash(id));
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

bool AbstractNode::parsePackage(const BasePackage &pkg, AbstractNodeInfo *sender) {

    if (!pkg.isValid()) {
        QuasarAppUtils::Params::verboseLog("incomming package is not valid!",
                                           QuasarAppUtils::Error);
        changeTrust(sender->id(), CRITICAL_ERROOR);
        return false;
    }

    if (!sender->isValid()) {
        QuasarAppUtils::Params::verboseLog("sender socket is not valid!",
                                           QuasarAppUtils::Error);
        changeTrust(sender->id(), LOGICK_ERROOR);
        return false;
    }

    emit incomingReques(pkg, sender->id());

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
        if (i.value().info.isBaned()) {
            list.push_back(QHostAddress(i.key()).toString());
        }
    }

    return list;
}

int AbstractNode::connectionsCount() const {
    int count = 0;
    for (auto i : _connections) {
        if (i.info.sct()) {
            if (!i.info.sct()->isValid()) {
                QuasarAppUtils::Params::verboseLog("connection count, findet not valid socket",
                                                   QuasarAppUtils::Warning);
            }

            count++;
        }
    }
    return count;
}

bool AbstractNode::isBaned(QAbstractSocket *socket) const {
    auto info = getInfo(socket->peerAddress());

    if (!info.isValid()) {
        return false;
    }

    return info.isBaned();
}

void AbstractNode::incomingConnection(qintptr handle) {

    if (useSSL) {
        incomingSsl(handle);
    } else {
        incomingTcp(handle);
    }
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

void AbstractNode::incomingSsl(qintptr socketDescriptor) {
    QSslSocket *serverSocket = new QSslSocket;

    serverSocket->setProtocol(QSsl::TlsV1_3);
    serverSocket->setLocalCertificate(QSslCertificate());

    if (serverSocket->setSocketDescriptor(socketDescriptor)) {
        connect(serverSocket, &QSslSocket::encrypted, [this, serverSocket](){
            registerSocket(serverSocket);
        });

        connect(serverSocket, QOverload<const QList<QSslError> &>::of(&QSslSocket::sslErrors),
            [serverSocket](const QList<QSslError> &errors){

                for (auto &error : errors) {
                    QuasarAppUtils::Params::verboseLog(error.errorString(), QuasarAppUtils::Error);
                }

                serverSocket->deleteLater();
        });

        serverSocket->startServerEncryption();
    } else {
        delete serverSocket;
    }
}

void AbstractNode::incomingTcp(qintptr socketDescriptor) {
    QTcpSocket *serverSocket = new QTcpSocket;
    if (serverSocket->setSocketDescriptor(socketDescriptor)) {
        connect(serverSocket, &QSslSocket::encrypted, [this, serverSocket](){
            registerSocket(serverSocket);
        });

        connect(serverSocket, QOverload<const QList<QSslError> &>::of(&QSslSocket::sslErrors),
            [serverSocket](const QList<QSslError> &errors){

                for (auto &error : errors) {
                    QuasarAppUtils::Params::verboseLog(error.errorString(), QuasarAppUtils::Error);
                }

                serverSocket->deleteLater();
        });

        serverSocket->startServerEncryption();
    } else {
        delete serverSocket;
    }
}


void AbstractNode::avelableBytes() {

    auto client = dynamic_cast<QAbstractSocket*>(sender());

    if (!client) {
        return;
    }

    auto id = qHash(client->peerAddress());

    if (!_connections.contains(id)) {
        return;
    }

    auto &val = _connections[id];

    auto array = client->readAll();
    if (val.pkg.hdr.isValid()) {
        val.pkg.data.append(array);

    } else {
        val.pkg.reset();

        memcpy(&val.pkg.hdr,
               array.data(), sizeof(BaseHeader));

        val.pkg.data.append(array.mid(sizeof(BaseHeader)));
    }

    if (val.pkg.isValid()) {
        parsePackage(val.pkg, &val.info);
    }

    if (val.pkg.data.size() >= val.pkg.hdr.size) {
        val.pkg.reset();
    }
}

void AbstractNode::handleDisconnected() {
    auto _sender = dynamic_cast<QTcpSocket*>(sender());

    if (_sender) {
        // log error

        auto ptr = getInfoPtr(_sender->peerAddress());
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
