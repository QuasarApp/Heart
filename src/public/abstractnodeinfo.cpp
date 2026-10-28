/*
 * Copyright (C) 2018-2025 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "abstractnodeinfo.h"
#include <hostaddress.h>
#include <QAbstractSocket>
#include <QDataStream>
#include <QHostInfo>
#include <QMetaObject>
#include <quasarapp.h>
#include <iparser.h>

namespace QH {

AbstractNodeInfo::AbstractNodeInfo(QAbstractSocket *sct,
                                   const HostAddress *address) {
    setSct(sct);
    if (address)
        setNetworkAddress(*address);

}

AbstractNodeInfo::~AbstractNodeInfo() {
    AbstractNodeInfo::removeSocket();
}

QAbstractSocket *AbstractNodeInfo::sct() const {
    return _sct;
}

void AbstractNodeInfo::removeSocket() {
    if (_sct) {
        auto socketPtr = _sct;
        _sct = nullptr;

        socketPtr->disconnect();

        // We invoke the delete later method on another thread (thread of the socket.)
        // because all network sockets must be open and closed in the one thread.

        QMetaObject::invokeMethod(socketPtr,
                                  "handleDisconnectFromHost",
                                  Qt::QueuedConnection);

        QMetaObject::invokeMethod(socketPtr,
                                  "deleteLater",
                                  Qt::QueuedConnection);

        setStatus(NodeConnectionStatus::NotConnected);
        emit sigDisconnected(this);

    }
}

void AbstractNodeInfo::ban() {
    _trust = static_cast<int>(TrustNode::Baned);
    removeSocket();
}

bool AbstractNodeInfo::isBanned() const {
    return _trust < 1;
}

void AbstractNodeInfo::unBan() {
    _trust = static_cast<int>(TrustNode::Restore);
}

void AbstractNodeInfo::setSct(QAbstractSocket *sct) {

    AbstractNodeInfo::removeSocket();

    _sct = sct;

    if (!_sct)
        return;


    if (!_sct->peerAddress().isNull()) {
        setNetworkAddress(HostAddress{_sct->peerAddress(), _sct->peerPort()});
    }

    auto connectedF = [this] () {

        setStatus(NodeConnectionStatus::Connected);
        emit sigConnected(this);
    };

    connect(_sct, &QAbstractSocket::connected,
            this, connectedF, Qt::DirectConnection);

    connect(_sct, &QAbstractSocket::disconnected,
            this, &AbstractNodeInfo::removeSocket, Qt::QueuedConnection);

    connect(_sct, &QAbstractSocket::errorOccurred,
            this, [this] (QAbstractSocket::SocketError err){
        emit sigErrorOccurred(this, err, _sct->errorString());
    }, Qt::DirectConnection);

    connect(_sct, &QAbstractSocket::readyRead,
            this, [this] (){ emit sigReadyRead(this);},
            Qt::DirectConnection);

    if (_sct->state() == QAbstractSocket::ConnectedState) {
        connectedF();
    }
}

void AbstractNodeInfo::setIsLocal(bool isLocal) {
    _isLocal = isLocal;
}

NodeConnectionStatus AbstractNodeInfo::status() const {
    return _status;
}

void AbstractNodeInfo::setStatus(const NodeConnectionStatus &status) {
    if (status == _status) {
        return;
    }

    _status = status;

    emit statusChanged(this, _status);
}

bool AbstractNodeInfo::confirmData() const {
    return _status != NodeConnectionStatus::NotConnected && fVersionDelivered() && fVersionReceived();
}

const PackagesVersionData& AbstractNodeInfo::multiVersionPackages() const {
    return _multiVersionPackages;
}

void AbstractNodeInfo::setMultiVersionPackages(const PackagesVersionData &newMultiVersionPackages) {
    _multiVersionPackages = newMultiVersionPackages;
}

bool AbstractNodeInfo::fVersionDelivered() const {
    return _fVersionDelivered;
}

void AbstractNodeInfo::setFVersionDelivered(bool newFVersionDelivered) {
    _fVersionDelivered = newFVersionDelivered;
}

bool AbstractNodeInfo::fVersionReceived() const {
    return _fVersionReceived;
}

void AbstractNodeInfo::setFVersionReceived(bool newFVersionReceived) {
    _fVersionReceived = newFVersionReceived;
}

const VersionData &AbstractNodeInfo::version() const {
    return _version;
}

void AbstractNodeInfo::setVersion(const VersionData &newVersion) {
    _version = newVersion;
}

void AbstractNodeInfo::updateConfirmStatus() {
    if (confirmData())
        setStatus(NodeConnectionStatus::Confirmed);
}

bool AbstractNodeInfo::isLocal() const {
    return _isLocal;
}

HostAddress AbstractNodeInfo::networkAddress() const {
    return _networkAddress;
}

void AbstractNodeInfo::setNetworkAddress(const HostAddress &networkAddress) {

    if (!networkAddress.isNull()) {
        _networkAddress = networkAddress;

        QHostInfo::lookupHost(_networkAddress.toString(), [this] (QHostInfo info){
            if (dynamic_cast<AbstractNodeInfo*>(this)) {
                setInfo(info);
            }
        });
    }
}

void AbstractNodeInfo::setInfo(const QHostInfo &info) {
    if (!_info)
        _info = new QHostInfo();

    *_info = info;
}

QHostInfo *AbstractNodeInfo::info() const {
    return _info;
}

int AbstractNodeInfo::trust() const {
    return _trust;
}

void AbstractNodeInfo::setTrust(int trust) {
    _trust = trust;

    if (isBanned() && _sct) {
        qDebug() << "The node" << _sct->peerAddress().toString() << "is banned!";

        removeSocket();
    }
}

bool AbstractNodeInfo::isValid() const {
    return _sct;
}

bool AbstractNodeInfo::isConnected() const {
    return isValid() && status() != NodeConnectionStatus::NotConnected;
}

void AbstractNodeInfo::reset() {
    setSct(nullptr);
    setNetworkAddress(HostAddress{});
    setTrust(static_cast<int>(TrustNode::Default));
    setStatus(NodeConnectionStatus::NotConnected);
    setIsLocal(false);

    QMutexLocker lock(&_parsersListMutex);
    _parsersMap.clear();

}

QSharedPointer<QH::iParser> AbstractNodeInfo::getParser(unsigned short cmd) {
    QMutexLocker lock(&_parsersListMutex);

    return _parsersMap.value(cmd, nullptr);
}

QSharedPointer<iParser> AbstractNodeInfo::getParser(const QString &type) {
    QMutexLocker lock(&_parsersListMutex);

    return _parsersKeysMap.value(type, nullptr);
}

void QH::AbstractNodeInfo::addParser(unsigned short cmd,
                                     QSharedPointer<QH::iParser> parser) {
    if (!parser)
        return;

    QMutexLocker lock(&_parsersListMutex);

    _parsersMap[cmd] = parser;
    _parsersKeysMap[parser->parserId()] = parser;
}

void AbstractNodeInfo::addParser(QSharedPointer<iParser> parser) {
    if (!parser)
        return;

    QMutexLocker lock(&_parsersListMutex);

    _parsersKeysMap[parser->parserId()] = parser;
}

uint qHash(NodeConnectionStatus status) {
    return static_cast<uint>(status);
}

}
