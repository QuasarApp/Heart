/*
 * Copyright (C) 2018-2022 QuasarApp.
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

        // We invoke the delate later method on another thread (thread of the socket.)
        // because all network sockets must be open adn closed in the one thread.

        QMetaObject::invokeMethod(socketPtr,
                                  "handleDisckonnetFromHost",
                                  Qt::QueuedConnection);

        QMetaObject::invokeMethod(socketPtr,
                                  "deleteLater",
                                  Qt::QueuedConnection);

        setStatus(NodeCoonectionStatus::NotConnected);
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

        setStatus(NodeCoonectionStatus::Connected);
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

NodeCoonectionStatus AbstractNodeInfo::status() const {
    return _status;
}

void AbstractNodeInfo::setStatus(const NodeCoonectionStatus &status) {
    if (status == _status) {
        return;
    }

    _status = status;

    emit statusChaned(this, _status);
}

bool AbstractNodeInfo::confirmData() const {
    return _status != NodeCoonectionStatus::NotConnected && fVersionDelivered() && fVersionReceived();
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
        setStatus(NodeCoonectionStatus::Confirmed);
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

    if (isBanned()) {
        QuasarAppUtils::Params::log(QString("The node %0 is banned!").
                                    arg(_sct->peerAddress().toString()));

        removeSocket();
    }
}

bool AbstractNodeInfo::isValid() const {
    return _sct;
}

bool AbstractNodeInfo::isConnected() const {
    return isValid() && status() != NodeCoonectionStatus::NotConnected;
}

void AbstractNodeInfo::reset() {
    setSct(nullptr);
    setNetworkAddress(HostAddress{});
    setTrust(static_cast<int>(TrustNode::Default));
    setStatus(NodeCoonectionStatus::NotConnected);
    setIsLocal(false);

}

uint qHash(NodeCoonectionStatus status) {
    return static_cast<uint>(status);
}

}
