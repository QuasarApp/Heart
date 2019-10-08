#include "server.h"
#include "quasarapp.h"
#include <QTcpSocket>
#include <factorynetobjects.h>
#include <pubkey.h>
#include "clientprotocol.h"

namespace ClientProtocol {

bool Server::parsePackage(const Package &pkg, QTcpSocket* sender) {
    if (!pkg.isValid()) {
        QuasarAppUtils::Params::verboseLog("incomming package is not valid!");
        changeKarma(sender->peerAddress().toIPv4Address(), CRITICAL_ERROOR);
        return false;
    }

    if (!sender->isValid()) {
        QuasarAppUtils::Params::verboseLog("incomming package is not valid!");
        changeKarma(sender->peerAddress().toIPv4Address(), LOGICK_ERROOR);
        return false;
    }

    switch (static_cast<Command>(pkg.hdr.command)) {
    case Command::Ping: {

        if (static_cast<Type>(pkg.hdr.type) != Type::Request) {
            return false;
        }

        Package pcg;

        if (!(pcg.create(Command::Ping, Type::Responke, &pkg.hdr))) {
            return false;
        };

        if (!sendPackage(pcg, sender)) {
            QuasarAppUtils::Params::verboseLog("!responce not sendet!");
        }
        break;
    }

    default: {

        emit incomingReques(pkg.hdr, pkg.data,
                            sender->peerAddress().toIPv4Address());
    }
    }

    return true;
}

bool Server::sendPackage(const Package &pkg, QTcpSocket * target) {
    if (!pkg.isValid()) {
        return false;
    }

    if (!target || !target->isValid()) {
        qCritical() << "destination server not valid!";
        return false;
    }

    if (!target->waitForConnected()) {
        qCritical() << "no connected to server! " << target->errorString();
        return false;
    }

    auto bytes = pkg.toBytes();
    bool sendet = bytes.size() == target->write(bytes);

    return sendet;
}

void Server::ban(quint32 target) {
    if (!_connections[target]) {
        _connections[target] = new Connectioninfo();
    }

    _connections[target]->ban();
}

void Server::unBan(quint32 target) {
    if (!_connections.contains(target)) {
        return;
    }

    _connections[target]->unBan();
}

bool Server::registerSocket(QTcpSocket *socket) {
    auto address = socket->peerAddress().toIPv4Address();

    if (!_pool) {
        QuasarAppUtils::Params::verboseLog("key pool is not inited", QuasarAppUtils::Error);
        return false;
    }

    RSAKeyPair pair;

    if (!_pool->take(BASE_RSA_BITS, pair)) {
        QuasarAppUtils::Params::verboseLog("key pool is empty", QuasarAppUtils::Error);
        return false;
    }

    _connections[address] = new Connectioninfo(socket, DEFAULT_KARMA,
                                           pair);

    connect(socket, &QTcpSocket::readyRead, this, &Server::avelableBytes);
    connect(socket, &QTcpSocket::disconnected, this, &Server::handleDisconected);

    if (!sendPubKey(socket, pair.pub)) {
        QuasarAppUtils::Params::verboseLog("not sendet pub key to client"
                                           "generate new key!", QuasarAppUtils::Error);
        return false;
    }

    return true;
}

bool Server::changeKarma(quint32 addresss, int diff) {
    auto ptr = _connections.value(addresss);
    if (!ptr) {
        return false;
    }

    auto objKarma = ptr->getKarma();

    if (objKarma >= NOT_VALID_CARMA) {
        return false;
    }

    if (objKarma <= BANED_KARMA) {
        return false;
    }

    ptr->setKarma(objKarma + diff);
    return true;
}

bool Server::isBaned(const QTcpSocket * adr) const {
    auto ptr = _connections.value(adr->peerAddress().toIPv4Address());

    if (!ptr) {
        return false;
    }

    return ptr->isBaned();
}

int Server::connectionsCount() const {
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

bool Server::sendPubKey(QTcpSocket * target, const QByteArray &pubKey) {

    Package pcg;

    PubKey pubkey;

    pubkey.setKey(pubKey);
    pubkey.setTypeKey(BASE_RSA_BITS);
    pubkey.setId(0);

    if (!pubkey.isValid()) {
        return false;
    }

    if (!(pcg.create(&pubkey, Type::Request))) {
        return false;
    };

    return sendPackage(pcg, target);

}

void Server::avelableBytes() {
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
               array.data(), sizeof(Header));

        _downloadPackage.data.append(array.mid(sizeof(Header)));
    }

    if (_downloadPackage.isValid()) {
        parsePackage(_downloadPackage, client);
    }

    if (_downloadPackage.data.size() >= _downloadPackage.hdr.size) {
        _downloadPackage.reset();
    }
}

void Server::handleDisconected() {
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

void Server::handleIncommingConnection() {
    while (hasPendingConnections()) {
        auto socket = nextPendingConnection();

        if (isBaned(socket)) {
            socket->abort();
            continue;
        }

        registerSocket(socket);
    }
}

Server::Server(RSAKeysPool *pool, QObject *ptr) :
    QTcpServer (ptr) {

    _pool = pool;
    connect(this, &Server::newConnection, this, &Server::handleIncommingConnection);
}

Server::~Server() {
    stop();
}

bool Server::run(const QString &ip, unsigned short port) {

    if (!listen(QHostAddress(ip), port)) {
        QuasarAppUtils::Params::verboseLog("listing fail " + this->errorString(),
                                           QuasarAppUtils::Error);
        return false;
    }

    return true;
}

void Server::stop(bool reset) {
    close();

    if (reset) {

        for (auto &&i : _connections) {
            i->disconnect();
        }

        _connections.clear();
    }
}

void Server::badRequest(quint32 address, const Header &req) {
    auto client = _connections.value(address);

    if (!client) {

        QuasarAppUtils::Params::verboseLog("Bad request detected, bud responce command not sendet!"
                                           " because client == null",
                                           QuasarAppUtils::Error);
        return;
    }

    if (!changeKarma(address, REQUEST_ERROR)) {

        QuasarAppUtils::Params::verboseLog("Bad request detected, bud responce command not sendet!"
                                           " because karma not changed",
                                           QuasarAppUtils::Error);

        return;
    }

    Package pcg;
    if (!(pcg.create(Command::BadRequest, Type::Responke, &req))) {
        QuasarAppUtils::Params::verboseLog("Bad request detected, bud responce command not sendet!"
                                           " because package not created",
                                           QuasarAppUtils::Error);
    };

    if (!sendPackage(pcg, client->getSct())) {

        QuasarAppUtils::Params::verboseLog("Bad request detected, bud responce command not sendet!"
                                           " because karma not changed",
                                           QuasarAppUtils::Error);
        return;
    }

    QuasarAppUtils::Params::verboseLog("Bad request sendet to adderess: " +
                                       client->getSct()->peerAddress().toString(),
                                       QuasarAppUtils::Info);
}

bool Server::sendResponse(const BaseNetworkObject *resp, quint32 address, const Header *req) {

    Package pcg;

    if (!pcg.create(resp, Type::Responke, req)) {
        QuasarAppUtils::Params::verboseLog("Response not sent because package not created",
                                           QuasarAppUtils::Error);
    }

    return sendResponse(&pcg, address, req);
}

bool Server::sendResponse(Package *pcg, quint32 address, const Header *req) {
    pcg->signPackage(req);
    return sendResponse(*pcg, address);
}

bool Server::sendResponse(const Package &pcg, quint32 address)
{
    auto client = _connections.value(address);

    if (!client) {
        QuasarAppUtils::Params::verboseLog("Response not sent because client == null",
                                           QuasarAppUtils::Error);
        return false;
    }


    if (!sendPackage(pcg, client->getSct())) {
        QuasarAppUtils::Params::verboseLog("Response not sent!",
                                           QuasarAppUtils::Error);
        return false;
    }

    return true;
}

QString Server::getWorkState() const {
    if (isListening()) {
        if (hasPendingConnections())
            return "overload";
        else {
            return "Work";
        }
    }

    return "Not running";
}

QString Server::connectionState() const {
    return QString("%0 / %1").arg(connectionsCount()).arg(maxPendingConnections());
}

QStringList Server::baned() const {
    QStringList list = {};
    for (auto i = _connections.begin(); i != _connections.end(); ++i) {
        if (i.value()->isBaned()) {
            list.push_back(QHostAddress(i.key()).toString());
        }
    }

    return list;
}

bool Server::getRSA(quint32 key, RSAKeyPair& res) const {
    auto sct = _connections.value(key);
    if (sct) {
        res = sct->getRSAKey();
        return true;
    }

    return false;
}

QByteArray Server::getToken(quint32 address) const {
    return _connections.value(address)->getToken();
}

bool Server::setToken(quint32 address, const QByteArray &token) {
    if (_connections.contains(address)) {
        _connections[address]->setToken(token);
        return true;
    }

    return false;
}


}
