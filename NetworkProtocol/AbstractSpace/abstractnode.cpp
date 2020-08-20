/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "abstractnode.h"
#include "datasender.h"
#include "ping.h"
#include "qsecretrsa2048.h"
#include "workstate.h"
#include <QHostInfo>
#include <QSslCertificate>
#include <QSslKey>
#include <QSslSocket>
#include <badrequest.h>
#include <qrsaencryption.h>
#include <quasarapp.h>
#include <openssl/rsa.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <QMetaObject>
#include <QtConcurrent>

namespace NP {

AbstractNode::AbstractNode(SslMode mode, QObject *ptr):
    QTcpServer(ptr) {

    _mode = mode;

    _dataSender = new DataSender();

    setMode(_mode);
}

bool AbstractNode::run(const QString &addres, unsigned short port) {
    HostAddress adr(addres, port);
    if (addres.isEmpty()) {
        adr = HostAddress{QHostAddress::Any};
    }

    if (!listen(adr)) {
        QuasarAppUtils::Params::log("Run fail " + this->errorString(),
                                    QuasarAppUtils::Error);
        return false;
    }

    return true;
}

void AbstractNode::stop() {
    close();

    QMutexLocker locer(&_connectionsMutex);
    for (auto &&i : _connections) {
        i->disconnect();
    }

    for (auto it: _workers) {
        if (!it->isFinished())
            it->cancel();
    }
}

AbstractNodeInfo *AbstractNode::getInfoPtr(const HostAddress &id) {
    QMutexLocker locer(&_connectionsMutex);

    if (!_connections.contains(id)) {
        return nullptr;
    }

    return dynamic_cast<AbstractNodeInfo*>(_connections[id]);
}

// fix me, if getInfoPtr invoced in an another thread and in some time main thread remove this object then
// this method return invalid object and application crashed.
const AbstractNodeInfo *AbstractNode::getInfoPtr(const HostAddress &id) const {
    QMutexLocker locer(&_connectionsMutex);

    if (!_connections.contains(id)) {
        return nullptr;
    }

    return dynamic_cast<AbstractNodeInfo*>(_connections[id]);
}

void AbstractNode::ban(const HostAddress &target) {
    auto info = getInfoPtr(target);
    if (info)
        info->ban();

}

void AbstractNode::unBan(const HostAddress &target) {
    QMutexLocker locer(&_connectionsMutex);

    if (!_connections.contains(target) || _connections[target]) {
        return;
    }

    _connections[target]->unBan();
}

bool AbstractNode::connectToHost(const HostAddress &address, SslMode mode) {
    QTcpSocket *socket;
    if (mode == SslMode::NoSSL) {
        socket = new QTcpSocket(nullptr);
    } else {
        socket = new QSslSocket(nullptr);
    }

    if (!registerSocket(socket, &address)) {
        return false;
    }

    socket->connectToHost(address, address.port());

    return true;
}

bool AbstractNode::connectToHost(const QString &domain, unsigned short port, SslMode mode) {
    QHostInfo::lookupHost(domain, [this, port, mode, domain](QHostInfo info) {

        if (info.error() != QHostInfo::NoError) {
            QuasarAppUtils::Params::log("The domain name :" + domain + " has error: " + info.errorString(),
                                        QuasarAppUtils::Error);
            return;
        }

        if (info.addresses().size() > 1) {
            QuasarAppUtils::Params::log("The domain name :" + domain + " has more 1 ip addresses.",
                                        QuasarAppUtils::Warning);
        }


        if (!connectToHost(HostAddress{info.addresses().first(), port}, mode)) {
            return;
        }

        auto hostObject = getInfoPtr(HostAddress{info.addresses().first(), port});

        if (!hostObject) {
            QuasarAppUtils::Params::log("The domain name :" + domain + " has connected bud not have network object!",
                                        QuasarAppUtils::Error);
            return;
        }

        hostObject->setInfo(info);
    });

    return true;
}

void AbstractNode::addNode(const HostAddress &nodeAdderess) {

    QMetaObject::invokeMethod(this,
                              "connectNodePrivate",
                              Qt::QueuedConnection,
                              Q_ARG(NP::HostAddress, nodeAdderess));

}

void AbstractNode::removeNode(const HostAddress &nodeAdderess) {

    for (int status = static_cast<int>(NodeCoonectionStatus::NotConnected);
         status < static_cast<int>(NodeCoonectionStatus::Confirmed); ++status) {

        takeFromQueue(nodeAdderess, static_cast<NodeCoonectionStatus>(status));
    }

    if (AbstractNodeInfo *ptr = getInfoPtr(nodeAdderess)) {
        ptr->disconnect();
    }
}

HostAddress AbstractNode::address() const {
    return HostAddress{serverAddress(), serverPort()};
}

AbstractNode::~AbstractNode() {
    //    delete _nodeKeys;
    stop();
}

QSslConfiguration AbstractNode::getSslConfig() const {
    return _ssl;
}

bool AbstractNode::generateRSAforSSL(EVP_PKEY *pkey) const {
    RSA * rsa = nullptr;
    if (!pkey) {
        return false;
    }

    if (!RSA_generate_key_ex(rsa, 2048, nullptr, nullptr)) {
        return false;
    }

    q_check_ptr(rsa);
    if (EVP_PKEY_assign_RSA(pkey, rsa) <= 0)
        return false;

    return true;
}

bool AbstractNode::generateSslDataPrivate(const SslSrtData &data, QSslCertificate& r_srt, QSslKey& r_key) {

    EVP_PKEY *pkey = EVP_PKEY_new();

    if (!generateRSAforSSL(pkey)) {
        return false;
    }

    X509 * x509 = nullptr;
    X509_NAME * name = nullptr;
    BIO * bp_public = nullptr, * bp_private = nullptr;
    const char *buffer = nullptr;
    int size;

    x509 = X509_new();
    q_check_ptr(x509);
    ASN1_INTEGER_set(X509_get_serialNumber(x509), 1);
    X509_gmtime_adj(X509_get_notBefore(x509), 0); // not before current time
    X509_gmtime_adj(X509_get_notAfter(x509), data.endTime); // not after a year from this point
    X509_set_pubkey(x509, pkey);
    name = X509_get_subject_name(x509);
    q_check_ptr(name);

    unsigned char *C = reinterpret_cast<unsigned char *>(data.country.toLatin1().data());
    X509_NAME_add_entry_by_txt(name, "C", MBSTRING_ASC, C, -1, -1, 0);

    unsigned char *O = reinterpret_cast<unsigned char *>(data.organization.toLatin1().data());
    X509_NAME_add_entry_by_txt(name, "O", MBSTRING_ASC, O, -1, -1, 0);

    unsigned char *CN = reinterpret_cast<unsigned char *>(data.commonName.toLatin1().data());
    X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC, CN, -1, -1, 0);

    X509_set_issuer_name(x509, name);
    X509_sign(x509, pkey, EVP_sha256());
    bp_private = BIO_new(BIO_s_mem());
    q_check_ptr(bp_private);
    if(PEM_write_bio_PrivateKey(bp_private, pkey, nullptr, nullptr, 0, nullptr, nullptr) != 1) {
        EVP_PKEY_free(pkey);
        X509_free(x509);
        BIO_free_all(bp_private);
        qCritical("PEM_write_bio_PrivateKey");
        return false;

    }

    bp_public = BIO_new(BIO_s_mem());
    q_check_ptr(bp_public);
    if(PEM_write_bio_X509(bp_public, x509) != 1){
        EVP_PKEY_free(pkey);
        X509_free(x509);
        BIO_free_all(bp_public);
        BIO_free_all(bp_private);
        qCritical("PEM_write_bio_PrivateKey");
        return false;

    }

    size = static_cast<int>(BIO_get_mem_data(bp_public, &buffer));
    q_check_ptr(buffer);

    r_srt = QSslCertificate(QByteArray(buffer, size));

    if(r_srt.isNull()) {
        EVP_PKEY_free(pkey);
        X509_free(x509);
        BIO_free_all(bp_public);
        BIO_free_all(bp_private);
        qCritical("Failed to generate a random client certificate");
        return false;

    }

    size = static_cast<int>(BIO_get_mem_data(bp_private, &buffer));
    q_check_ptr(buffer);
    r_key = QSslKey(QByteArray(buffer, size), QSsl::Rsa);
    if(r_key.isNull()) {
        EVP_PKEY_free(pkey);
        X509_free(x509);
        BIO_free_all(bp_public);
        BIO_free_all(bp_private);
        qCritical("Failed to generate a random private key");
        return false;

    }

    EVP_PKEY_free(pkey); // this will also free the rsa key
    X509_free(x509);
    BIO_free_all(bp_public);
    BIO_free_all(bp_private);

    return true;
}

QSslConfiguration AbstractNode::selfSignedSslConfiguration() {
    QSslConfiguration res = QSslConfiguration::defaultConfiguration();

    QSslKey pkey;
    QSslCertificate crt;
    SslSrtData sslData;

    if (!generateSslDataPrivate(sslData, crt, pkey)) {

        QuasarAppUtils::Params::log("fail to create ssl certificate. node svitch to InitFromSystem mode",
                                    QuasarAppUtils::Warning);

        return res;
    }

    res.setPrivateKey(pkey);
    res.setLocalCertificate(crt);

    return res;
}

AbstractNodeInfo *AbstractNode::createNodeInfo(QAbstractSocket *socket) const {
    return new AbstractNodeInfo(socket);
}

bool AbstractNode::registerSocket(QAbstractSocket *socket, const HostAddress* clientAddress) {

    if (connectionsCount() >= maxPendingConnections()) {
        return false;
    }

    auto info = createNodeInfo(socket);

    _connectionsMutex.lock();
    HostAddress cliAddress;
    if (clientAddress)
        cliAddress = *clientAddress;
    else
        cliAddress = info->networkAddress();

    _connections[cliAddress] = info;

    _connectionsMutex.unlock();

    connect(socket, &QAbstractSocket::readyRead, this, &AbstractNode::avelableBytes);
    connect(socket, &QAbstractSocket::disconnected, this, &AbstractNode::handleDisconnected,
            Qt::QueuedConnection);

    connect(socket, &QAbstractSocket::connected, this, &AbstractNode::handleConnected,
            Qt::QueuedConnection);


    // check node confirmed
    QTimer::singleShot(WAIT_CONFIRM_TIME, this,
                       std::bind(&AbstractNode::handleCheckConfirmendOfNode, this, cliAddress));

    connectionRegistered(info);

    return true;
}

ParserResult AbstractNode::parsePackage(const Package &pkg,
                                        const AbstractNodeInfo *sender) {

    if (!(sender && sender->isValid())) {
        QuasarAppUtils::Params::log("sender socket is not valid!",
                                    QuasarAppUtils::Error);
        return ParserResult::Error;
    }

    if (!pkg.isValid()) {
        QuasarAppUtils::Params::log("incomming package is not valid!",
                                    QuasarAppUtils::Error);
        changeTrust(sender->networkAddress(), CRITICAL_ERROOR);
        return ParserResult::Error;
    }

    if (H_16<Ping>() == pkg.hdr.command) {
        Ping cmd(pkg);

        if (!cmd.ansver()) {
            cmd.setAnsver(true);
            sendData(&cmd, sender->networkAddress(), &pkg.hdr);
        }

        incomingData(&cmd, sender->networkAddress());
        return ParserResult::Processed;
    } else if (H_16<BadRequest>() == pkg.hdr.command) {
        BadRequest cmd(pkg);

        incomingData(&cmd, sender->networkAddress());
        emit requestError(cmd.err());

        return ParserResult::Processed;

    }

    return ParserResult::NotProcessed;
}

bool AbstractNode::sendPackage(const Package &pkg, QAbstractSocket *target) const {
    if (!pkg.isValid()) {
        return false;
    }

    if (!target || !target->isValid()) {
        QuasarAppUtils::Params::log("destination server not valid!",
                                    QuasarAppUtils::Error);
        return false;
    }

    if (!target->waitForConnected()) {
        QuasarAppUtils::Params::log("no connected to server! " + target->errorString(),
                                    QuasarAppUtils::Error);
        return false;
    }

    return QMetaObject::invokeMethod(const_cast<DataSender*>(_dataSender),
                                     "sendPackagePrivate",
                                     Qt::QueuedConnection,
                                     Q_ARG(QByteArray, pkg.toBytes()),
                                     Q_ARG(void*, target));

}

bool AbstractNode::sendData(const AbstractData *resp,
                            const HostAddress &addere,
                            const Header *req) {
    auto client = getInfoPtr(addere);

    if (!client) {
        QuasarAppUtils::Params::log("Response not sent because client == null",
                                    QuasarAppUtils::Error);
        return false;
    }

    if (!resp) {
        return false;
    }

    Package pkg;
    bool convert = false;
    if (req) {
        convert = resp->toPackage(pkg, req->command);
    } else {
        convert = resp->toPackage(pkg);
    }

    if (!convert) {
        QuasarAppUtils::Params::log("Response not sent because dont create package from object",
                                    QuasarAppUtils::Error);
        return false;
    }

    if (!sendPackage(pkg, client->sct())) {
        QuasarAppUtils::Params::log("Response not sent!",
                                    QuasarAppUtils::Error);
        return false;
    }

    return true;
}

void AbstractNode::badRequest(const HostAddress &address, const Header &req,
                              const QString msg) {
    auto client = getInfoPtr(address);

    if (!client) {

        QuasarAppUtils::Params::log("Bad request detected, bud responce command not sendet!"
                                    " because client == null",
                                    QuasarAppUtils::Error);
        return;
    }

    if (!changeTrust(address, REQUEST_ERROR)) {

        QuasarAppUtils::Params::log("Bad request detected, bud responce command not sendet!"
                                    " because trust not changed",
                                    QuasarAppUtils::Error);

        return;
    }

    auto bad = BadRequest(msg);
    if (!sendData(&bad, address, &req)) {
        return;
    }

    QuasarAppUtils::Params::log("Bad request sendet to adderess: " +
                                client->sct()->peerAddress().toString(),
                                QuasarAppUtils::Info);
}

WorkState AbstractNode::getWorkState() const {
    WorkState state;

    state.setConnectionCount(connectionsCount());
    state.setMaxConnectionCount(maxPendingConnections());
    state.setBanedList(banedList());

    return state;

}

QString AbstractNode::pareseResultToString(const ParserResult &parseResult) const {
    switch (parseResult)
    {
    case ParserResult::Processed:
        return "Processed";
    case ParserResult::NotProcessed:
        return "NotProcessed";

    default: return "Error";
    }
}

QString AbstractNode::getWorkStateString() const {
    if (isListening()) {
        if (connectionsCount() >= maxPendingConnections())
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

QList<HostAddress> AbstractNode::banedList() const {
    QList<HostAddress> list = {};

    QMutexLocker locer(&_connectionsMutex);

    for (auto i = _connections.begin(); i != _connections.end(); ++i) {
        if (i.value()->isBaned()) {
            list.push_back(i.key());
        }
    }

    return list;
}

int AbstractNode::connectionsCount() const {
    int count = 0;

    QMutexLocker locer(&_connectionsMutex);

    for (auto i : _connections) {
        if (i->isConnected()) {
            count++;
        }
    }
    return count;
}

bool AbstractNode::ping(const HostAddress &address) {
    Ping cmd;
    return sendData(&cmd, address);

}

bool AbstractNode::isBaned(QAbstractSocket *socket) const {
    auto info = getInfoPtr(HostAddress{socket->peerAddress(), socket->peerPort()});

    if (!(info && info->isValid())) {
        return false;
    }

    return info->isBaned();
}

void AbstractNode::incomingConnection(qintptr handle) {

    if (_mode == SslMode::NoSSL) {
        incomingTcp(handle);
    } else {
        incomingSsl(handle);
    }
}

bool AbstractNode::changeTrust(const HostAddress &id, int diff) {
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
    QSslSocket *socket = new QSslSocket;

    socket->setSslConfiguration(_ssl);

    if (!isBaned(socket) && socket->setSocketDescriptor(socketDescriptor)) {
        connect(socket, &QSslSocket::encrypted, [this, socket](){
            if (!registerSocket(socket)) {
                socket->deleteLater();
            }
        });

        connect(socket, QOverload<const QList<QSslError> &>::of(&QSslSocket::sslErrors),
                [socket](const QList<QSslError> &errors){

            for (auto &error : errors) {
                QuasarAppUtils::Params::log(error.errorString(), QuasarAppUtils::Error);
            }

            socket->deleteLater();
        });

        socket->startServerEncryption();
    } else {
        delete socket;
    }
}

void AbstractNode::incomingTcp(qintptr socketDescriptor) {
    QTcpSocket *socket = new QTcpSocket;
    if (!isBaned(socket) && socket->setSocketDescriptor(socketDescriptor)) {
        if (!registerSocket(socket)) {
            delete socket;
        }
    } else {
        delete socket;
    }
}


void AbstractNode::avelableBytes() {

    auto client = dynamic_cast<QAbstractSocket*>(sender());

    if (!client) {
        return;
    }

//    bug!!!!, peer port is random value or 0
    auto id = HostAddress{client->peerAddress(), client->peerPort()};

    if (!_connections.contains(id)) {
        return;
    }

    auto &pkg = _packages[id];

    auto array = client->readAll();
    if (pkg.hdr.isValid()) {
        pkg.data.append(array);

    } else {
        pkg.reset();

        memcpy(&pkg.hdr,
               array.data(), sizeof(Header));

        pkg.data.append(array.mid(sizeof(Header)));
    }

    if (pkg.isValid())
        newWork(pkg, getInfoPtr(id), id);

    if (pkg.data.size() >= pkg.hdr.size) {
        pkg.reset();
    }
}

/// @todo create a new system of drop error nodes.
/// if node closed by error then this node need to reconnect. bud if node longer time in disconnect statuse then drop this node.
///
void AbstractNode::handleDisconnected() {
    auto _sender = dynamic_cast<QTcpSocket*>(sender());

    if (_sender) {
        // log error

        auto ptr = getInfoPtr(HostAddress{_sender->peerAddress(), _sender->peerPort()});
        if (!ptr) {
            QuasarAppUtils::Params::log("system error in void Server::handleDisconected()"
                                        " address not valid",
                                        QuasarAppUtils::Error);
            return;
        }

        ptr->setStatus(NodeCoonectionStatus::NotConnected);
        nodeStatusChanged(ptr->networkAddress(), NodeCoonectionStatus::NotConnected);
        ptr->disconnect();

        return;
    }

    QuasarAppUtils::Params::log("system error in void Server::handleDisconected()"
                                "dynamic_cast fail!",
                                QuasarAppUtils::Error);
}

void AbstractNode::handleConnected() {

    auto _sender = dynamic_cast<QTcpSocket*>(sender());

    if (_sender) {

        auto ptr = getInfoPtr(HostAddress{_sender->peerAddress(), _sender->peerPort()});

        if (!ptr) {
            QuasarAppUtils::Params::log("system error in void Server::handleDisconected()"
                                        " address not valid",
                                        QuasarAppUtils::Error);
            return;
        }

        ptr->setStatus(NodeCoonectionStatus::Connected);
        nodeStatusChanged(ptr->networkAddress(), NodeCoonectionStatus::Connected);

        return;
    }

    QuasarAppUtils::Params::log("system error in void Server::handleDisconected()"
                                "dynamic_cast fail!",
                                QuasarAppUtils::Error);
}

void AbstractNode::nodeConfirmet(const HostAddress& node) {

    auto ptr = getInfoPtr(node);

    if (!ptr) {
        QuasarAppUtils::Params::log("system error in void Server::handleDisconected()"
                                    " address not valid",
                                    QuasarAppUtils::Error);
    }

    ptr->setStatus(NodeCoonectionStatus::Confirmed);
    nodeStatusChanged(ptr->networkAddress(), NodeCoonectionStatus::Confirmed);
}

void AbstractNode::handleCheckConfirmendOfNode(HostAddress node) {
    checkConfirmendOfNode(node);
}

void AbstractNode::handleWorkerStoped() {
    auto senderObject = dynamic_cast<QFutureWatcher <bool>*>(sender());

    if (senderObject) {
       _workers.remove(senderObject);
       delete senderObject;
    }
}

bool AbstractNode::listen(const HostAddress &address) {
    return QTcpServer::listen(address, address.port());
}

void AbstractNode::connectNodePrivate(HostAddress address) {

    AbstractNodeInfo* info = getInfoPtr(address);
    if (!(info && info->isConnected())) {
        connectToHost(address, _mode);
    }
}

void AbstractNode::newWork(const Package &pkg, const AbstractNodeInfo *sender,
                           const HostAddress& id) {

    if (!sender)
        return;

    auto executeObject = [pkg, sender, id, this]() {
        ParserResult parseResult = parsePackage(pkg, sender);

        if (parseResult != ParserResult::Processed) {
            auto message = QString("Package not parsed! result: '%3'."
                                   " header: size(%0) command(%1) triggerCommnad(%2).").
                    arg(pkg.hdr.size).
                    arg(pkg.hdr.command).
                    arg(pkg.hdr.triggerCommnad).
                    arg(pareseResultToString(parseResult));

            QuasarAppUtils::Params::log(message, QuasarAppUtils::Warning);

            if (parseResult == ParserResult::NotProcessed) {
                changeTrust(id, REQUEST_ERROR);
            }

            return false;
        }

        bool fConfirmed = sender->confirmData();
        if (fConfirmed && sender->status() != NodeCoonectionStatus::Confirmed) {
            nodeConfirmet(id);
        }

        return true;
    };

    auto worker = new QFutureWatcher <bool>();
    worker->setFuture(QtConcurrent::run(executeObject));
    _workers.insert(worker);

    connect(worker, &QFutureWatcher<bool>::finished,
            this, &AbstractNode::handleWorkerStoped);
}

SslMode AbstractNode::getMode() const {
    return _mode;
}

bool AbstractNode::setMode(const SslMode &mode) {

    if (_mode == mode) {
        return true;
    }

    if (isListening()) {
        return false;
    }

    _mode = mode;

    switch (_mode) {
    case SslMode::InitFromSystem: {
        _ssl = QSslConfiguration::defaultConfiguration();
        break;

    }
    case SslMode::InitSelfSigned: {
        _ssl = selfSignedSslConfiguration();
        break;

    }
    default: {
        _ssl = QSslConfiguration();
        break;
    }

    }

    return true;

}

void AbstractNode::incomingData(AbstractData *pkg, const HostAddress &sender) {
    Q_UNUSED(pkg)
    Q_UNUSED(sender)

}

QHash<HostAddress, AbstractNodeInfo *> AbstractNode::connections() const {
    return _connections;
}

void AbstractNode::connectionRegistered(const AbstractNodeInfo *info) {
    Q_UNUSED(info);
}

void AbstractNode::pushToQueue(const std::function<void()>& action,
                               const HostAddress &node,
                               NodeCoonectionStatus triggerStatus) {

    _actionCacheMutex.lock();
    _actionCache[node][triggerStatus].push_back(action);
    _actionCacheMutex.unlock();

}

QList<std::function<void ()> >
AbstractNode::takeFromQueue(const HostAddress &node,
                            NodeCoonectionStatus triggerStatus) {

    _actionCacheMutex.lock();

    auto list = _actionCache[node][triggerStatus];
    _actionCache[node].remove(triggerStatus);

    if (_actionCache[node].size() == 0)
        _actionCache.remove(node);

    _actionCacheMutex.unlock();

    return list;
}

void AbstractNode::nodeStatusChanged(const HostAddress &node, NodeCoonectionStatus status) {

    auto list = takeFromQueue(node, status);

    for (const auto &action : list) {
        action();
    }

    if (status == NodeCoonectionStatus::NotConnected) {
        nodeDisconnected(node);
    } else if (status == NodeCoonectionStatus::Connected) {
        nodeConnected(node);
    } else if (status == NodeCoonectionStatus::Confirmed) {
        nodeConfirmet(node);
    }
}

void AbstractNode::nodeConfirmend(const HostAddress &node) {
    Q_UNUSED(node);
}

void AbstractNode::nodeConnected(const HostAddress &node) {
    Q_UNUSED(node);
}

void AbstractNode::nodeDisconnected(const HostAddress &node) {
    Q_UNUSED(node);
}

void AbstractNode::checkConfirmendOfNode(const HostAddress &node) {
    auto info = getInfoPtr(node);

    if(!info)
        return;

    if (info->status() != NodeCoonectionStatus::Confirmed) {
        removeNode(node);
    }
}

}
