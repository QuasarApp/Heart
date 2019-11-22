#include "abstractnode.h"
#include "workstate.h"
#include <QSslCertificate>
#include <QSslKey>
#include <QSslKey>
#include <QSslSocket>
#include <badrequest.h>
#include <quasarapp.h>
#include <openssl/rsa.h>
#include <openssl/x509.h>
#include <openssl/pem.h>

namespace NetworkProtocol {

AbstractNode::AbstractNode(SslMode mode, QObject *ptr):
    QTcpServer(ptr) {
    _mode = mode;

    setMode(_mode);
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
        i.info->disconnect();
    }
}

QWeakPointer<AbstractNodeInfo> AbstractNode::getInfoPtr(const QHostAddress &id) {
    if (!_connections.contains(id)) {
        return {nullptr};
    }

    return _connections[id].info;
}

AbstractNodeInfo AbstractNode::getInfo(const QHostAddress &id) const{
    auto info = _connections.value(id).info;

    if (info.isNull())
        return {};

    return *_connections.value(id).info.get();
}

void AbstractNode::ban(const QHostAddress &target) {

    auto info = getInfoPtr(target);

    if (info.isNull())
        _connections[target] = NodeInfoData{};

    _connections[target].info->ban();
}

void AbstractNode::unBan(const QHostAddress &target) {
    if (!_connections.contains(target) || _connections[target].info.isNull()) {
        return;
    }

    _connections[target].info->unBan();
}

void AbstractNode::connectToHost(const QHostAddress &ip, unsigned short port, SslMode mode) {
    QTcpSocket *socket;
    if (mode == SslMode::NoSSL) {
        socket = new QTcpSocket(nullptr);
    } else {
        socket = new QSslSocket(nullptr);
    }

    registerSocket(socket, &ip);
    socket->connectToHost(ip, port);
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

        QuasarAppUtils::Params::verboseLog("fail to create ssl certificate. node svitch to InitFromSystem mode",
                                           QuasarAppUtils::Warning);

        return res;
    }

    res.setPrivateKey(pkey);
    res.setLocalCertificate(crt);

    return res;
}

QSharedPointer<AbstractNodeInfo> AbstractNode::createNodeInfo(QAbstractSocket *socket) const {
    return QSharedPointer<AbstractNodeInfo>::create(socket);
}

bool AbstractNode::registerSocket(QAbstractSocket *socket, const QHostAddress* clientAddress) {

    if (connectionsCount() >= maxPendingConnections()) {
        return false;
    }

    auto info = createNodeInfo(socket);

    if (clientAddress)
        _connections[*clientAddress] = {info, {}};
    else
        _connections[info->id()] = {info, {}};


    connect(socket, &QAbstractSocket::readyRead, this, &AbstractNode::avelableBytes);
    connect(socket, &QAbstractSocket::disconnected, this, &AbstractNode::handleDisconnected);

    return true;
}

ParserResult AbstractNode::parsePackage(const Package &pkg, QWeakPointer<AbstractNodeInfo> sender) {

    auto senderPtr = sender.toStrongRef();

    if (senderPtr.isNull() || !senderPtr->isValid()) {
        QuasarAppUtils::Params::verboseLog("sender socket is not valid!",
                                           QuasarAppUtils::Error);
        changeTrust(senderPtr->id(), LOGICK_ERROOR);
        return ParserResult::Error;
    }

    if (!pkg.isValid()) {
        QuasarAppUtils::Params::verboseLog("incomming package is not valid!",
                                           QuasarAppUtils::Error);
        changeTrust(senderPtr->id(), CRITICAL_ERROOR);
        return ParserResult::Error;
    }



    return ParserResult::NotProcessed;
}

bool AbstractNode::sendPackage(const Package &pkg, QAbstractSocket *target) {
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

bool AbstractNode::sendData(const QWeakPointer<AbstractData> resp, const QHostAddress &addere,
                                const Header *req) {
    auto client = getInfoPtr(addere).toStrongRef();

    if (client.isNull()) {
        QuasarAppUtils::Params::verboseLog("Response not sent because client == null",
                                           QuasarAppUtils::Error);
        return false;
    }

    auto responce = resp.toStrongRef();
    if (responce.isNull()) {
        return false;
    }

    Package pkg;
    bool convert = false;
    if (req) {
        convert = responce->toPackage(pkg, req->command);
    } else {
        convert = responce->toPackage(pkg);
    }

    if (!convert) {
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

void AbstractNode::badRequest(const QHostAddress &address, const Header &req,
                              const QString msg) {
    auto client = getInfoPtr(address).toStrongRef();

    if (client.isNull()) {

        QuasarAppUtils::Params::verboseLog("Bad request detected, bud responce command not sendet!"
                                           " because client == null",
                                           QuasarAppUtils::Error);
        return;
    }

    if (!changeTrust(address, REQUEST_ERROR)) {

        QuasarAppUtils::Params::verboseLog("Bad request detected, bud responce command not sendet!"
                                           " because trust not changed",
                                           QuasarAppUtils::Error);

        return;
    }

    auto bad = BadRequest(msg);
    Package pcg;

    if (!bad.toPackage(pcg, req.command)) {
        QuasarAppUtils::Params::verboseLog("Bad request detected, bud responce command not sendet!"
                                           " because package not created",
                                           QuasarAppUtils::Error);
    }

    if (!sendPackage(pcg, client->sct())) {

        QuasarAppUtils::Params::verboseLog("Bad request detected, bud responce command not sendet!"
                                           " because karma not changed",
                                           QuasarAppUtils::Error);
        return;
    }

    QuasarAppUtils::Params::verboseLog("Bad request sendet to adderess: " +
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

QList<QHostAddress> AbstractNode::banedList() const {
    QList<QHostAddress> list = {};
    for (auto i = _connections.begin(); i != _connections.end(); ++i) {
        if (i.value().info->isBaned()) {
            list.push_back(i.key());
        }
    }

    return list;
}

int AbstractNode::connectionsCount() const {
    int count = 0;
    for (auto i : _connections) {
        if (i.info->sct()) {
            if (!i.info->sct()->isValid()) {
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

    if (_mode == SslMode::NoSSL) {
        incomingTcp(handle);
    } else {
        incomingSsl(handle);
    }
}

bool AbstractNode::changeTrust(const QHostAddress &id, int diff) {
    auto ptr = getInfoPtr(id).toStrongRef();
    if (ptr.isNull()) {
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
                    QuasarAppUtils::Params::verboseLog(error.errorString(), QuasarAppUtils::Error);
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

    auto id = client->peerAddress();

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
               array.data(), sizeof(Header));

        val.pkg.data.append(array.mid(sizeof(Header)));
    }

    if (val.pkg.isValid()) {
        parsePackage(val.pkg, val.info);
    }

    if (val.pkg.data.size() >= val.pkg.hdr.size) {
        val.pkg.reset();
    }
}

void AbstractNode::handleDisconnected() {
    auto _sender = dynamic_cast<QTcpSocket*>(sender());

    if (_sender) {
        // log error

        auto ptr = getInfoPtr(_sender->peerAddress()).toStrongRef();
        if (!ptr.isNull()) {
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

}
