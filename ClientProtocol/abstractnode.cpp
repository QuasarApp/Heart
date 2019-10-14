#include "abstractdata.h"
#include "abstractnode.h"
#include <QSslCertificate>
#include <QSslKey>
#include <QSslKey>
#include <QSslSocket>
#include <quasarapp.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/x509.h>
#include <openssl/pem.h>

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

bool AbstractNode::generateSslData(QSslCertificate& srt, QSslKey& key) {


    EVP_PKEY * pkey = nullptr;
    RSA * rsa = nullptr;
    X509 * x509 = nullptr;
    X509_NAME * name = nullptr;
    BIO * bp_public = nullptr, * bp_private = nullptr;
    const char * buffer = nullptr;
    int size;

    pkey = EVP_PKEY_new();
    q_check_ptr(pkey);
    rsa = RSA_generate_key(2048, RSA_F4, nullptr, nullptr);
    q_check_ptr(rsa);
    EVP_PKEY_assign_RSA(pkey, rsa);
    x509 = X509_new();
    q_check_ptr(x509);
    ASN1_INTEGER_set(X509_get_serialNumber(x509), 1);
    X509_gmtime_adj(X509_get_notBefore(x509), 0); // not before current time
    X509_gmtime_adj(X509_get_notAfter(x509), 31536000L); // not after a year from this point
    X509_set_pubkey(x509, pkey);
    name = X509_get_subject_name(x509);
    q_check_ptr(name);
    X509_NAME_add_entry_by_txt(name, "C", MBSTRING_ASC, (unsigned char *)"US", -1, -1, 0);
    X509_NAME_add_entry_by_txt(name, "O", MBSTRING_ASC, (unsigned char *)"My Organization", -1, -1, 0);
    X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC, (unsigned char *)"My Common Name", -1, -1, 0);
    X509_set_issuer_name(x509, name);
    X509_sign(x509, pkey, EVP_sha1());
    bp_private = BIO_new(BIO_s_mem());
    q_check_ptr(bp_private);
    if(PEM_write_bio_PrivateKey(bp_private, pkey, nullptr, nullptr, 0, nullptr, nullptr) != 1)
    {
        EVP_PKEY_free(pkey);
        X509_free(x509);
        BIO_free_all(bp_private);
        qCritical("PEM_write_bio_PrivateKey");
        return false;

    }
    bp_public = BIO_new(BIO_s_mem());
    q_check_ptr(bp_public);
    if(PEM_write_bio_X509(bp_public, x509) != 1)
    {
        EVP_PKEY_free(pkey);
        X509_free(x509);
        BIO_free_all(bp_public);
        BIO_free_all(bp_private);
        qCritical("PEM_write_bio_PrivateKey");
        return false;

    }
    size = static_cast<int>(BIO_get_mem_data(bp_public, &buffer));
    q_check_ptr(buffer);

    srt = QSslCertificate(QByteArray(buffer, size));

    if(srt.isNull())
    {
        qCritical("Failed to generate a random client certificate");
        return false;

    }

    size = static_cast<int>(BIO_get_mem_data(bp_private, &buffer));
    q_check_ptr(buffer);
    key = QSslKey(QByteArray(buffer, size), QSsl::Rsa);
    if(key.isNull())
    {
        qCritical("Failed to generate a random private key");
        return false;

    }

    EVP_PKEY_free(pkey); // this will also free the rsa key
    X509_free(x509);
    BIO_free_all(bp_public);
    BIO_free_all(bp_private);

    return true;
}

AbstractNode::~AbstractNode() {
    stop();
}

bool AbstractNode::registerSocket(QAbstractSocket *socket) {

    if (connectionsCount() >= maxPendingConnections()) {
        return false;
    }

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

QString AbstractNode::getWorkState() const {
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
    QSslSocket *socket = new QSslSocket;

    socket->setProtocol(QSsl::TlsV1_3);
    socket->setLocalCertificate(QSslCertificate());

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
