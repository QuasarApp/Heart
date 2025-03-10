/*
 * Copyright (C) 2018-2025 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "abstractnode.h"
#include "datasender.h"
#include "ping.h"
#include "workstate.h"
#include <QHostInfo>

#include <badrequest.h>
#include <quasarapp.h>
#include <QTcpServer>

#ifdef USE_HEART_SSL

#include <sslsocket.h>
#include <easyssl/x509.h>
#include <easyssl/rsassl.h>

#include <QSslConfiguration>
#include <QSslCertificate>
#include <QSslKey>
#include <QSslSocket>

#endif

#include <QMetaObject>
#include <QtConcurrent>
#include <closeconnection.h>
#include "tcpsocket.h"
#include "asynclauncher.h"
#include "receivedata.h"
#include "abstracttask.h"

#include <apiversion.h>
#include <versionisreceived.h>
#include <taskscheduler.h>
#include <qaglobalutils.h>
#include <bigdatawraper.h>
#include <bigdataparser.h>
#include <abstractnodeparser.h>
#include <apiversionparser.h>

namespace QH {

using namespace PKG;

AbstractNode::AbstractNode( QObject *ptr):
    QTcpServer(ptr) {

    initThreadId();

    _senderThread = new QThread();
    _senderThread->setObjectName("Sender");

    _senderThread->start();

    // This object moving to _senderThread.
    _dataSender = new DataSender(_senderThread);
    _socketWorker = new AsyncLauncher(_senderThread);
    _tasksheduller = new TaskScheduler();
    _apiVersionParser = new APIVersionParser(this);

    addApiParser<BigDataParser>();

    auto abstractNodeParser = addApiParserNative<AbstractNodeParser>();
    connect(abstractNodeParser.data(), &AbstractNodeParser::sigPingReceived,
            this, &AbstractNode::receivePing, Qt::DirectConnection);

    connect(_apiVersionParser, &APIVersionParser::sigNoLongerSupport,
            this, &AbstractNode::sigNoLongerSupport, Qt::DirectConnection);

    qRegisterMetaType<QSharedPointer<QH::AbstractTask>>();
#ifdef USE_HEART_SSL
    qRegisterMetaType<QList<QSslError>>();
#endif

    connect(_tasksheduller, &TaskScheduler::sigPushWork,
            this, &AbstractNode::handleBeginWork);


    initThreadPool();

}

AbstractNode::~AbstractNode() {

    _senderThread->quit();
    _senderThread->wait();

    for (auto it: std::as_const(_receiveData)) {
        delete  it;
    }

    _receiveData.clear();

    delete _dataSender;
    delete _senderThread;
    delete _socketWorker;
    delete _tasksheduller;
    delete _apiVersionParser;
}

bool AbstractNode::run(const QString &addres, unsigned short port) {

    if (!port)
        return false;

    HostAddress adr(addres, port);
    if (addres.isEmpty()) {
        adr = HostAddress{QHostAddress::Any, port};
    }

    if (!listen(adr)) {

        qCritical() << "Run fail " << this->errorString();
        qCritical() << "Address:: " << adr.toString();

        return false;
    }

    initThreadPool();

    return true;
}

QSharedPointer<iParser> AbstractNode::selectParser(unsigned short cmd,
                                                   AbstractNodeInfo *sender) const {
    return _apiVersionParser->selectParser(cmd, sender);
}

QSharedPointer<iParser> AbstractNode::selectParser(const QString &type,
                                                   AbstractNodeInfo *sender) const {
    return _apiVersionParser->selectParser(type, sender);
}

QSharedPointer<iParser> AbstractNode::selectParser(const QString &type, int version) const {
    return _apiVersionParser->selectParser(type, version);
}

void AbstractNode::stop() {
    close();

    _connectionsMutex.lock();
    for (const auto &i : std::as_const(_connections)) {
        i->disconnect();
    }
    _connectionsMutex.unlock();

    _workersMutex.lock();
    for (auto it: std::as_const(_workers)) {
        if (!it->isFinished()) {
            it->cancel();
            it->waitForFinished();
        }
    }
    _workersMutex.unlock();

    deinitThreadPool();
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

bool AbstractNode::addNode(const HostAddress &address) {

    AsyncLauncher::Job action = [this, address]() -> bool {
        QAbstractSocket *socket;
#ifdef USE_HEART_SSL
        if (_mode == SslMode::NoSSL) {
            socket = new TcpSocket(nullptr);
        } else {
            socket = new SslSocket(nullptr);
        }

#else
        socket = new TcpSocket(nullptr);
#endif

        if (!registerSocket(socket, &address)) {
            addNodeFailed(AddNodeError::RegisterSocketFailed);
            delete socket;
            return false;
        }

        socket->connectToHost(address, address.port());

        return true;
    };

    return _socketWorker->run(action);
}

bool AbstractNode::addNode(const HostAddress &address,
                           const std::function<void (QH::AbstractNodeInfo *)> &action,
                           NodeCoonectionStatus status) {

    auto peer = getInfoPtr(address);

    if (action && (!peer || peer->status() < status)) {
        auto &actionsList = _connectActions[status];
        actionsList[address] = action;
    }

    return addNode(address);
}

bool AbstractNode::addNode(const QString &domain, unsigned short port,
                           const std::function<void (QH::AbstractNodeInfo *)> &action,
                           NodeCoonectionStatus status) {

    HostAddress address{domain, port};
    if (address.isNull()) {

        QHostInfo::lookupHost(domain, [this, port, domain, action, status](QHostInfo info) {

            if (info.error() != QHostInfo::NoError) {

                qCritical() << "The domain name :" + domain +
                               " has error: " + info.errorString();
                addNodeFailed(AddNodeError::HostNotFound);
                return;
            }

            auto addresses = info.addresses();

            if (addresses.size() > 1) {
                qWarning() << "The domain name :" + domain +
                              " has more 1 ip addresses.";
            }

            if (action) {
                addNode(HostAddress{addresses.first(), port}, action, status);
            } else {
                addNode(HostAddress{addresses.first(), port});
            }

        });


        return true;
    }


    return addNode(address, action);
}

bool AbstractNode::removeNode(const HostAddress &nodeAdderess) {

    if (AbstractNodeInfo *ptr = getInfoPtr(nodeAdderess)) {
        return removeNode(ptr);
    }

    return false;
}

bool AbstractNode::removeNode(AbstractNodeInfo *node) {
    if (!(node)) {
        return false;
    }

    if (!node->isConnected())
        return true;

    if (node->isLocal()) {
        node->removeSocket();
        return true;
    }

    QTimer::singleShot(WAIT_CONFIRM_TIME, this,
                       std::bind(&AbstractNode::handleForceRemoveNode,
                                 this, node->networkAddress()));

    CloseConnection close;
    return sendData(&close, node);
}

HostAddress AbstractNode::address() const {
    return HostAddress{serverAddress(), serverPort()};
}

#ifdef USE_HEART_SSL
QSslConfiguration AbstractNode::getSslConfig() const {
    return _ssl;
}

QSslConfiguration AbstractNode::selfSignedSslConfiguration(const EasySSL::SslSrtData &sslData) {
    QSslConfiguration res = QSslConfiguration::defaultConfiguration();

    QSslKey pkey;
    QSslCertificate crt;

    EasySSL::X509 generator(QSharedPointer<EasySSL::RSASSL>::create());
    EasySSL::SelfSignedSertificate certificate = generator.create(sslData);
    res.setPrivateKey(certificate.key);
    res.setLocalCertificate(certificate.crt);
    res.setPeerVerifyMode(QSslSocket::VerifyNone);

    return res;
}

bool AbstractNode::configureSslSocket(AbstractNodeInfo *node, bool fServer) {

    if (!node)
        return false;

    auto socket = dynamic_cast<SslSocket*>(node->sct());
    if (!socket) {

        QuasarAppUtils::Params::log("Invalid ssl socket!! Connection not secure",
                                    QuasarAppUtils::Error);
        return false;
    }

    socket->setSslConfiguration(_ssl);

    auto address = node->networkAddress();
    connect(socket, &QSslSocket::encrypted, this ,[this, address]() {
        handleEncrypted(getInfoPtr(address));
    });

    connect(socket, &SslSocket::sslErrorsOcurred,
            this, &AbstractNode::handleSslErrorOcurredPrivate, Qt::DirectConnection);


    AsyncLauncher::Job action = [socket, fServer]() -> bool {
        if (fServer)
            socket->startServerEncryption();
        else
            socket->startClientEncryption();

        return true;
    };

    return _socketWorker->run(action);
}

const QList<QSslError> &AbstractNode::ignoreSslErrors() const {
    return _ignoreSslErrors;
}

void AbstractNode::setIgnoreSslErrors(const QList<QSslError> &newIgnoreSslErrors) {
    _ignoreSslErrors = newIgnoreSslErrors;
};

bool AbstractNode::useSelfSignedSslConfiguration(const EasySSL::SslSrtData &crtData) {

    if (isListening()) {
        return false;
    }

    _ssl = selfSignedSslConfiguration(crtData);
    _mode = SslMode::InitSelfSigned;

    if(!_ignoreSslErrors.contains(QSslError{QSslError::SelfSignedCertificate}))
        _ignoreSslErrors.push_back(QSslError{QSslError::SelfSignedCertificate});

    if(!_ignoreSslErrors.contains(QSslError{QSslError::SelfSignedCertificateInChain}))
        _ignoreSslErrors.push_back(QSslError{QSslError::SelfSignedCertificateInChain});

    return !_ssl.isNull();
}

bool AbstractNode::useSystemSslConfiguration(QSslConfiguration config) {
    if (isListening()) {
        return false;
    }

    _ssl = config;
    _mode = SslMode::InitFromSystem;

    return !_ssl.isNull();
}

bool AbstractNode::disableSSL() {
    if (isListening()) {
        return false;
    }

    _mode = SslMode::NoSSL;

    return true;
}

void AbstractNode::handleEncrypted(AbstractNodeInfo *node) {
    handleNodeStatusChanged(node, NodeCoonectionStatus::Connected);
}

void AbstractNode::handleSslErrorOcurredPrivate(SslSocket * sslScocket, const QList<QSslError> &errors) {

    QList<QSslError> ignore;
    for (auto &error : errors) {

        if (!_ignoreSslErrors.contains(QSslError{error.error()})) {
            handleSslErrorOcurred(sslScocket, error);
        } else {
            ignore += error;
        }
    }

    if (ignore.isEmpty())
        return;

    if (sslScocket) {
        sslScocket->ignoreSslErrors(ignore);
    }
}


void AbstractNode::handleSslErrorOcurred(SslSocket *scket,
                                         const QSslError &error) {
    QuasarAppUtils::Params::log(scket->peerAddress().toString() + " : " + error.errorString(),
                                QuasarAppUtils::Error);

    QuasarAppUtils::Params::log("Error code: " + QString::number(error.error()),
                                QuasarAppUtils::Error);
}

#endif

bool AbstractNode::fCloseConnectionAfterBadRequest() const {
    return _closeConnectionAfterBadRequest;
}

void AbstractNode::setCloseConnectionAfterBadRequest(bool newCloseConnectionAfterBadRequest) {
    _closeConnectionAfterBadRequest = newCloseConnectionAfterBadRequest;
}

bool AbstractNode::fSendBadRequestErrors() const {
    return _sendBadRequestErrors;
}

void AbstractNode::setSendBadRequestErrors(bool val) {
    _sendBadRequestErrors = val;
}

const QSharedPointer<iParser> &
AbstractNode::addApiParserImpl(const QSharedPointer<iParser> &parserObject) {
    return _apiVersionParser->addApiParser(parserObject);
}

AbstractNodeInfo *AbstractNode::createNodeInfo(QAbstractSocket *socket,
                                               const HostAddress* clientAddress) const {
    return new AbstractNodeInfo(socket, clientAddress);
}

bool AbstractNode::registerSocket(QAbstractSocket *socket, const HostAddress* clientAddress) {

    if (connectionsCount() >= maxPendingConnections()) {
        return false;
    }

    HostAddress cliAddress;
    if (clientAddress)
        cliAddress = *clientAddress;
    else
        cliAddress = HostAddress{socket->peerAddress(), socket->peerPort()};


    _connectionsMutex.lock();

    if (_connections.contains(cliAddress)) {
        auto info =_connections.value(cliAddress);
        info->setSct(socket);
        info->setIsLocal(clientAddress);

        _connectionsMutex.unlock();

        if (!info->isValid()) {
            return false;
        }

        nodeAddedSucessful(info);
        return true;
    }

    auto info = createNodeInfo(socket, &cliAddress);

    info->setIsLocal(clientAddress);

    _connections[cliAddress] = info;

    _connectionsMutex.unlock();

    connect(info, &AbstractNodeInfo::sigReadyRead,
            this, &AbstractNode::avelableBytes, Qt::DirectConnection);

    // using direct connection because socket clear all data of ip and port after disconnected.
    connect(info, &AbstractNodeInfo::statusChaned,
            this, &AbstractNode::handleNodeStatusChanged,
            Qt::QueuedConnection);

    if (info->status() != NodeCoonectionStatus::NotConnected) {
        handleNodeStatusChanged(info, info->status());
    }

    // using direct connection because socket clear all data of ip and port after disconnected.
    connect(info, &AbstractNodeInfo::sigErrorOccurred,
            this, &AbstractNode::nodeErrorOccured,
            Qt::QueuedConnection);

    // check node confirmed
    QTimer::singleShot(WAIT_TIME, this, [this, cliAddress]() {
        checkConfirmendOfNode(getInfoPtr(cliAddress));
    });

    nodeAddedSucessful(info);

    return true;
}

ParserResult AbstractNode::parsePackage(const QSharedPointer<AbstractData> &pkg,
                                        const Header &pkgHeader,
                                        AbstractNodeInfo *sender) {
    return _apiVersionParser->parsePackage(pkg, pkgHeader, sender);
}

QSharedPointer<AbstractData> AbstractNode::genPackage(unsigned short cmd,
                                                      AbstractNodeInfo *sender) const {
    if (_apiVersionParser)
        return _apiVersionParser->searchPackage(cmd, sender);

    return nullptr;
}

bool AbstractNode::sendPackage(const Package &pkg, QAbstractSocket *target) const {
    if (!pkg.isValid()) {
        return false;
    }

    if (!target || !target->isValid()) {

        qCritical() << "Destination server not valid!";
        return false;
    }

    if (!target->waitForConnected()) {
        qCritical() << "no connected to server! " + target->errorString();
        return false;
    }

    return _dataSender->sendData(pkg.toBytes(), target, true);
}

unsigned int AbstractNode::sendData(const AbstractData *resp,
                                    const HostAddress &addere,
                                    const Header *req) {
    return sendData(resp, getInfoPtr(addere), req);
}

unsigned int AbstractNode::sendData(const PKG::AbstractData *resp,
                                    const AbstractNodeInfo *node,
                                    const Header *req) {

    if (!node) {
        qDebug() << "Response not sent because client == null";
        return 0;
    }

    if (!resp) {
        return 0;
    }

    Package pkg;
    bool convert = false;
    if (req && req->isValid()) {
        convert = resp->toPackage(pkg, node->multiVersionPackages().value(resp->cmd()),req->hash);
    } else {
        convert = resp->toPackage(pkg, node->multiVersionPackages().value(resp->cmd()));
    }

    if (!convert) {

        if (static_cast<unsigned int>(pkg.data.size()) > Package::maximumSize()) {
            // big data

            auto wrap = QSharedPointer<BigDataWraper>::create();
            wrap->setData(resp);
            if ( parsePackage(wrap, {}, getInfoPtr(node->networkAddress())) != ParserResult::Processed) {
                return 0;
            }

            return BIG_DATA_HASH_ID;
        }
        qCritical() << "Response not sent because dont create package from object";
        return 0;
    }

    if (!sendPackage(pkg, node->sct())) {
        qCritical() << "Response not sent!";
        return 0;
    }

    return pkg.hdr.hash;
}

void AbstractNode::badRequest(const HostAddress &address, const Header &req,
                              const ErrorData &err, qint8 diff) {

    if (!changeTrust(address, diff)) {


        qCritical() << "Bad request detected, bud response command not sent!"
                       " because trust not changed";


        qCritical() << "SECURITY LOG: Force block the " + address.toString() +
                       " because trust defined";

        ban(address);

        return;
    }

    auto node = getInfoPtr(address);
    if (!isBanned(node)) {
        auto bad = BadRequest(err);
        if (!sendData(&bad, address, &req)) {
            return;
        }

        qInfo() << "Bad request sendet to adderess: " + address.toString();

        if (fCloseConnectionAfterBadRequest()) {
            removeNode(node);
        }
    }
}

WorkState AbstractNode::getWorkState() const {
    WorkState state;

    state.setConnections(connectionsList());
    state.setActiveConnections(activeConnectionsList());
    state.setMaxConnectionCount(maxPendingConnections());
    state.setBanedList(banedList());
    state.setIsRun(isListening());

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

QList<HostAddress> AbstractNode::banedList() const {
    QList<HostAddress> list = {};

    QMutexLocker locer(&_connectionsMutex);

    for (auto i = _connections.begin(); i != _connections.end(); ++i) {
        if (i.value()->isBanned()) {
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

int AbstractNode::confirmendCount() const {
    int count = 0;

    QMutexLocker locer(&_connectionsMutex);

    for (auto i : _connections) {
        if (i->status() == NodeCoonectionStatus::Confirmed) {
            count++;
        }
    }
    return count;
}

bool AbstractNode::ping(const HostAddress &address) {
    Ping cmd;
    return sendData(&cmd, address);

}

bool AbstractNode::isBanned(const AbstractNodeInfo *info) const {
    if (!(info && info->isValid())) {
        return false;
    }

    return info->isBanned();
}

void AbstractNode::incomingConnection(qintptr handle) {
    AsyncLauncher::Job action = [this, handle]() -> bool {
        QAbstractSocket* socket = nullptr;

#ifdef USE_HEART_SSL
        if (_mode == SslMode::NoSSL) {
            socket = new TcpSocket(nullptr);
        } else {
            socket = new SslSocket(nullptr);
        }

#else
        socket = new TcpSocket(nullptr);
#endif

        socket->setSocketDescriptor(handle);

        if (isBanned(getInfoPtr(HostAddress{socket->peerAddress(), socket->peerPort()}))) {
            qCritical() << "Income connection from banned address";

            delete socket;
            return false;
        }

        if (!registerSocket(socket)) {

            qCritical() << "Failed to register new socket";

            delete socket;
            return false;
        }

        return true;

    };

    _socketWorker->run(action);
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

void AbstractNode::avelableBytes(AbstractNodeInfo *sender) {

    if (!sender) {
        return;
    }

    auto id = sender->networkAddress();

    if (!_connections.contains(id)) {
        return;
    }

    if (!_receiveData.contains(id)) {
        _receiveData.insert(id, new ReceiveData());
    }

    auto &pkg = _receiveData[id]->_pkg;
    auto &hdrArray = _receiveData[id]->_hdrArray;

    int workIndex = 0;
    const int headerSize = sizeof(Header);

    auto socket = sender->sct();
    if (!socket) {
        pkg.reset();
        hdrArray.clear();
        return;
    }

    // concat with old data of header.
    auto array = hdrArray;
    while (socket->bytesAvailable() > 0) {
        array += socket->readAll();
    };

    const int arraySize = array.size();
    hdrArray.clear();

    while (arraySize > workIndex) {

        int offset = arraySize - workIndex;

        if (pkg.hdr.isValid()) {
            // CASE 1: The Package data is still not collected, but the header is already collected. performs full or partial filling of packet data.

            int dataLength = std::min(static_cast<int>(pkg.hdr.size - pkg.data.size()),
                                      arraySize - workIndex);
            pkg.data.append(array.mid(workIndex, dataLength));

            workIndex += dataLength;


        } else if (offset >= headerSize) {

            // CASE 2: The header and package still do not exist and the amount of data allows you to create a new header. A header is created and will fill in all or part of the package data.

            pkg.reset();

            memcpy(&pkg.hdr,
                   array.data() + workIndex, headerSize);

            if (!pkg.hdr.isValid())
                return;

            int dataLength = std::min(static_cast<int>(pkg.hdr.size),
                                      arraySize - headerSize - workIndex);

            pkg.data.append(array.mid(workIndex + headerSize, dataLength));

            workIndex += headerSize + dataLength;

        } else {
            // CASE 3: There is not enough data to initialize the header. The data will be placed in temporary storage and will be processed the next time the data is received.

            unsigned char dataLength = static_cast<unsigned char>(arraySize - workIndex);
            hdrArray += array.mid(workIndex, dataLength);
            workIndex += dataLength;
        }

        if (pkg.isValid()) {
            newWork(pkg, sender, id);
            pkg.reset();
            hdrArray.clear();
        } else if (static_cast<unsigned int>(pkg.data.size()) >= pkg.hdr.size) {
            qWarning() << "Invalid Package received." + pkg.toString();
            pkg.reset();
            hdrArray.clear();
            changeTrust(id, CRITICAL_ERROOR);

        }
    }
}

void AbstractNode::handleWorkerStoped() {
    auto senderObject = dynamic_cast<QFutureWatcher <bool>*>(sender());

    if (senderObject) {

        _workersMutex.lock();
        _workers.remove(senderObject);
        _workersMutex.unlock();

        delete senderObject;
    }
}

void AbstractNode::handleForceRemoveNode(HostAddress node) {
    AbstractNodeInfo* info = getInfoPtr(node);
    if (info) {
        info->removeSocket();
    }
}

void AbstractNode::handleBeginWork(QSharedPointer<QH::AbstractTask> work) {

    auto executeObject = [this, work]() -> bool {
        if (!work)
            return false;

        return work->execute(this);
    };

    QMutexLocker locer(&_threadPoolMutex);

    if (_threadPool) {
        auto worker = new QFutureWatcher <bool>();
        worker->setFuture(QtConcurrent::run(_threadPool, executeObject));

        _workersMutex.lock();
        _workers.insert(worker);
        _workersMutex.unlock();


        connect(worker, &QFutureWatcher<bool>::finished,
                this, &AbstractNode::handleWorkerStoped);
    }
}

bool AbstractNode::listen(const HostAddress &address) {
    return QTcpServer::listen(address, address.port());
}

QSharedPointer<AbstractData>
AbstractNode::prepareData(const Package &pkg,
                          AbstractNodeInfo *sender) const {

    auto value = _apiVersionParser->searchPackage(pkg.hdr.command, sender);
    if (!value) {
        qDebug() << "Package type not registered!"
                    " Please use the registerPackageType method before parsing."
                    " Example invoke registerPackageType<MyData>() into constructor of you client and server nodes.";

        return nullptr;
    }

    value->fromPakcage(pkg);
    return value;
}

QList<HostAddress> AbstractNode::connectionsList() const {
    QMutexLocker locer(&_connectionsMutex);

    return _connections.keys();
}

QList<HostAddress> AbstractNode::activeConnectionsList() const {

    QList<HostAddress> result;

    QMutexLocker locer(&_connectionsMutex);
    for (auto i : _connections) {
        if (i->isConnected()) {
            result.push_back(i->networkAddress());
        }
    }

    return result;
}

void AbstractNode::addNodeFailed(AddNodeError error) {

    switch (error) {
    case AddNodeError::HostNotFound: {
        qCritical() << "The remote host not found or dns server not responce.";
        break;

    }

    case AddNodeError::RegisterSocketFailed: {
        qCritical() << "The remote node is banned or serve is overload.";
        break;

    }
    default: {
        qCritical() << "The unknown error ocurred.";
    }
    }

}

void AbstractNode::nodeAddedSucessful(AbstractNodeInfo *) {

}

bool AbstractNode::addApiParser(const QSharedPointer<iParser> &parser) {

    if (!parser || parser->node() != this)
        return false;

    return !addApiParserImpl(parser).isNull();
}

void AbstractNode::receivePing(const QSharedPointer<PKG::Ping> &) {};

bool AbstractNode::sheduleTask(const QSharedPointer<AbstractTask> &task) {
    return _tasksheduller->shedule(task);
}

void AbstractNode::removeTask(int taskId) {
    _tasksheduller->remove(taskId);
}

int AbstractNode::sheduledTaskCount() const {
    return _tasksheduller->taskCount();
}

void AbstractNode::newWork(const Package &pkg, AbstractNodeInfo *sender,
                           const HostAddress& id) {

    if (!sender)
        return;

    auto executeObject = [pkg, sender, id, this]() {

        auto data = prepareData(pkg, sender);
        if (!data)
            return false;

        ParserResult parseResult = parsePackage(data, pkg.hdr, sender);

#ifdef HEART_PRINT_PACKAGES
        QuasarAppUtils::Params::log(QString("Package received! %0").arg(data->toString()), QuasarAppUtils::Info);
#endif

        if (parseResult != ParserResult::Processed) {

            auto message = QString("Package not parsed! %0 \nresult: %1. \n%2").
                           arg(pkg.toString(), iParser::pareseResultToString(parseResult), data->toString());

            qInfo() << message;

            if (parseResult == ParserResult::Error) {

                if (fSendBadRequestErrors()) {
                    badRequest(id, pkg.hdr, ErrorData{1, "Your send the invalid request."}, REQUEST_ERROR);
                } else {
                    changeTrust(id, REQUEST_ERROR);
                }
            }

            if (parseResult == ParserResult::NotProcessed) {
                if (fSendBadRequestErrors()) {
                    badRequest(id, pkg.hdr, ErrorData{2, "Your send the not supported command."}, NOTSUPPORT_ERROR);
                } else {
                    changeTrust(id, NOTSUPPORT_ERROR);
                }

            }

#ifdef QT_DEBUG
            qInfo() << _apiVersionParser->toString();
#endif

            return false;
        }

        _confirmNodeMutex.lock();
        sender->updateConfirmStatus();
        _confirmNodeMutex.unlock();


        return true;
    };


    QMutexLocker locer(&_threadPoolMutex);

    if (_threadPool) {
        auto worker = new QFutureWatcher <bool>();
        worker->setFuture(QtConcurrent::run(_threadPool, executeObject));

        _workersMutex.lock();
        _workers.insert(worker);
        _workersMutex.unlock();

        connect(worker, &QFutureWatcher<bool>::finished,
                this, &AbstractNode::handleWorkerStoped);
    }
}

SslMode AbstractNode::getMode() const {
    return _mode;
}

QHash<HostAddress, AbstractNodeInfo *> AbstractNode::connections() const {
    return _connections;
}

void AbstractNode::prepareForDelete() {
    stop();
}

void AbstractNode::handleNodeStatusChanged(AbstractNodeInfo *node, NodeCoonectionStatus status) {

    if (status == NodeCoonectionStatus::NotConnected) {
        nodeDisconnected(node);
    } else if (status == NodeCoonectionStatus::Connected) {

#ifdef USE_HEART_SSL
        if (_mode != SslMode::NoSSL) {

            auto socket = dynamic_cast<SslSocket*>(node->sct());

            if (!socket) {
                QuasarAppUtils::Params::log("Failed to preparet to configure ssl socket.",
                                            QuasarAppUtils::Error);

                removeNode(node);
                return;
            }

            if (!socket->isEncrypted()) {
                if (!configureSslSocket(node, !node->isLocal())) {
                    QuasarAppUtils::Params::log("Failed to configure ssl socket.",
                                                QuasarAppUtils::Error);
                }

                return;
            }
        }
#endif

        nodeConnected(node);
    } else if (status == NodeCoonectionStatus::Confirmed) {
        nodeConfirmend(node);
    }
}

void AbstractNode::nodeConfirmend(AbstractNodeInfo *node) {

    auto &actions = _connectActions[NodeCoonectionStatus::Confirmed];
    auto action = actions.take(node->networkAddress());
    if (action)
        action(node);
}

void AbstractNode::nodeConnected(AbstractNodeInfo *node) {
    if (!_apiVersionParser->sendSupportedAPI(node)) {
        qCritical() << "Failed to sent version information to dist node";
    }

    auto &actions = _connectActions[NodeCoonectionStatus::Connected];
    auto action = actions.take(node->networkAddress());
    if (action)
        action(node);
}

void AbstractNode::nodeDisconnected(AbstractNodeInfo *node) {
    Q_UNUSED(node)
}

void AbstractNode::nodeErrorOccured(AbstractNodeInfo *nodeInfo,
                                    QAbstractSocket::SocketError errorCode,
                                    QString errorString) {
    Q_UNUSED(errorCode)

    QString message("Network error occured on the %0 node. Message: %1");
    qCritical() << message.arg(nodeInfo->networkAddress().toString(), errorString);

    auto &actions = _connectActions[NodeCoonectionStatus::Connected];
    actions.remove(nodeInfo->networkAddress());
}

void AbstractNode::checkConfirmendOfNode(AbstractNodeInfo *info) {

    if(!info)
        return;

    if (info->status() != NodeCoonectionStatus::Confirmed) {
        removeNode(info);
    }
}

void AbstractNode::initThreadId() const {
    mainThreadID();
}

void AbstractNode::initThreadPool() {
    deinitThreadPool();

    QMutexLocker lock(&_threadPoolMutex);
    _threadPool = new QThreadPool();
    _threadPool->setObjectName("PackageWorker");
    _threadPool->setMaxThreadCount(QThread::idealThreadCount());

}

void AbstractNode::deinitThreadPool() {
    QMutexLocker lock(&_threadPoolMutex);

    if (!_threadPool) {
        return;
    }

    _threadPool->waitForDone(WAIT_TIME);
    delete _threadPool;
    _threadPool = nullptr;
}

QThread *AbstractNode::mainThreadID() {
    static auto thread = QThread::currentThread();

    return thread;
}

} // namespace QH
