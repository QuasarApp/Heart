/*
 * Copyright (C) 2018-2021 QuasarApp.
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

namespace QH {

AbstractNodeInfo::AbstractNodeInfo(QAbstractSocket *sct,
                                   const HostAddress *address) {
    setSct(sct);
    if (address)
        setNetworkAddress(*address);

}

AbstractNodeInfo::~AbstractNodeInfo() {}

QAbstractSocket *AbstractNodeInfo::sct() const {
    return _sct;
}

void AbstractNodeInfo::disconnect() {
    if (_sct) {
        auto socketPtr = _sct;

        // We invoke the delate later method on another thread (thread of the socket.)
        // because all network sockets must be open adn closed in the one thread.
        QMetaObject::invokeMethod(socketPtr,
                                  "deleteLater",
                                  Qt::QueuedConnection);

        _sct = nullptr;

        setStatus(NodeCoonectionStatus::NotConnected);
    }
}

void AbstractNodeInfo::ban() {
    _trust = static_cast<int>(TrustNode::Baned);
    disconnect();
}

bool AbstractNodeInfo::isBanned() const {
    return _trust < 1;
}

void AbstractNodeInfo::unBan() {
    _trust = static_cast<int>(TrustNode::Restore);
}

void AbstractNodeInfo::setSct(QAbstractSocket *sct) {

    AbstractNodeInfo::disconnect();

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

    auto diconnectedF = [this] () {

        disconnect();
        setStatus(NodeCoonectionStatus::NotConnected);
        emit sigDisconnected(this);
    };

    connect(_sct, &QAbstractSocket::connected,
            this, connectedF, Qt::DirectConnection);

    connect(_sct, &QAbstractSocket::disconnected,
            this, diconnectedF, Qt::DirectConnection);

    connect(_sct, &QAbstractSocket::errorOccurred,
            this, [this] (QAbstractSocket::SocketError err){emit sigErrorOccurred(this, err);},
            Qt::DirectConnection);

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
    return _status != NodeCoonectionStatus::NotConnected;
}

void AbstractNodeInfo::updateConfirmStatus() {
    if (confirmData())
        setStatus(NodeCoonectionStatus::Confirmed);
}

bool AbstractNodeInfo::isLocal() const {
    return _isLocal;
}

HostAddress AbstractNodeInfo::networkAddress() const {
    if (isValid() && _sct->isValid())
        return HostAddress{_sct->peerAddress(), _sct->peerPort()};

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
        disconnect();
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
