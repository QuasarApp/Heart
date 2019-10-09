#include "baseserver.h"
#include "quasarapp.h"
#include <QTcpSocket>
#include "clientprotocol.h"

namespace ClientProtocol {

bool BaseServer::parsePackage(const BasePackage &pkg, QAbstractSocket* sender) {
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

        BasePackage pcg;

        if (!(pcg.create(Command::Ping, Type::Responke, &pkg.hdr))) {
            return false;
        }

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

bool BaseServer::sendPackage(const BasePackage &pkg, QAbstractSocket * target) {
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

void BaseServer::ban(quint32 target) {
    if (!_connections[target]) {
        _connections[target] = new AbstractNode();
    }

    _connections[target]->ban();
}

void BaseServer::unBan(quint32 target) {
    if (!_connections.contains(target)) {
        return;
    }

    _connections[target]->unBan();
}

bool BaseServer::registerSocket(QAbstractSocket *socket) {
    auto address = socket->peerAddress().toIPv4Address();


    _connections[address] = new AbstractNode(socket, DEFAULT_KARMA);

    connect(socket, &QTcpSocket::readyRead, this, &BaseServer::avelableBytes);
    connect(socket, &QTcpSocket::disconnected, this, &BaseServer::handleDisconected);

    return true;
}

bool BaseServer::changeKarma(quint32 addresss, int diff) {
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

bool BaseServer::isBaned(const QTcpSocket * adr) const {
    auto ptr = _connections.value(adr->peerAddress().toIPv4Address());

    if (!ptr) {
        return false;
    }

    return ptr->isBaned();
}

int BaseServer::connectionsCount() const {
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

void BaseServer::avelableBytes() {
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

void BaseServer::handleDisconected() {
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

void BaseServer::handleIncommingConnection() {
    while (hasPendingConnections()) {
        auto socket = nextPendingConnection();

        if (isBaned(socket)) {
            socket->abort();
            continue;
        }

        registerSocket(socket);
    }
}

BaseServer::BaseServer(QObject *ptr) :
    QTcpServer (ptr) {

    connect(this, &BaseServer::newConnection, this, &BaseServer::handleIncommingConnection);
}

BaseServer::~BaseServer() {
    stop();
}

bool BaseServer::run(const QString &ip, unsigned short port) {

    if (!listen(QHostAddress(ip), port)) {
        QuasarAppUtils::Params::verboseLog("listing fail " + this->errorString(),
                                           QuasarAppUtils::Error);
        return false;
    }

    return true;
}

void BaseServer::stop(bool reset) {
    close();

    if (reset) {

        for (auto &&i : _connections) {
            i->disconnect();
        }

        _connections.clear();
    }
}

void BaseServer::badRequest(quint32 address, const BaseHeader &req) {
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

    BasePackage pcg;
    if (!(pcg.create(Command::BadRequest, Type::Responke, &req))) {
        QuasarAppUtils::Params::verboseLog("Bad request detected, bud responce command not sendet!"
                                           " because package not created",
                                           QuasarAppUtils::Error);
    }

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

bool BaseServer::sendResponse(const BaseNetworkObject *resp, quint32 address, const BaseHeader *req) {

    BasePackage pcg;

    if (!pcg.create(resp, Type::Responke, req)) {
        QuasarAppUtils::Params::verboseLog("Response not sent because package not created",
                                           QuasarAppUtils::Error);
    }

    return sendResponse(&pcg, address, req);
}

bool BaseServer::sendResponse(BasePackage *pcg, quint32 address, const BaseHeader *req) {
    pcg->signPackage(req);
    return sendResponse(*pcg, address);
}

bool BaseServer::sendResponse(const BasePackage &pcg, quint32 address)
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

QString BaseServer::getWorkState() const {
    if (isListening()) {
        if (hasPendingConnections())
            return "overload";
        else {
            return "Work";
        }
    }

    return "Not running";
}

QString BaseServer::connectionState() const {
    return QString("%0 / %1").arg(connectionsCount()).arg(maxPendingConnections());
}

QStringList BaseServer::baned() const {
    QStringList list = {};
    for (auto i = _connections.begin(); i != _connections.end(); ++i) {
        if (i.value()->isBaned()) {
            list.push_back(QHostAddress(i.key()).toString());
        }
    }

    return list;
}

QByteArray BaseServer::getToken(quint32 address) const {
    return _connections.value(address)->getToken();
}

bool BaseServer::setToken(quint32 address, const QByteArray &token) {
    if (_connections.contains(address)) {
        _connections[address]->setToken(token);
        return true;
    }

    return false;
}


}
