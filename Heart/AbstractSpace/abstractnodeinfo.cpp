/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "abstractnodeinfo.h"
#include <hostaddress.h>
#include <QAbstractSocket>
#include <QDataStream>
#include <QHostInfo>

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

void AbstractNodeInfo::disconnect(bool disableEvents) {
    if (_sct) {
        auto socketPtr = _sct;

        if (disableEvents)
            socketPtr->disconnect();

        _sct = nullptr;
        socketPtr->close();
        socketPtr->deleteLater();
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
    QObject::disconnect(_sct, nullptr, this, nullptr);

    _sct = sct;
    if (_sct && !_sct->peerAddress().isNull()) {
        setNetworkAddress(HostAddress{_sct->peerAddress(), _sct->peerPort()});
    }

    connect(_sct, &QAbstractSocket::connected,
            this, [this] (){ emit sigConnected(this);},
            Qt::DirectConnection);

    connect(_sct, &QAbstractSocket::disconnected,
            this, [this] (){ disconnect(), emit sigDisconnected(this);},
            Qt::DirectConnection);

    connect(_sct, &QAbstractSocket::errorOccurred,
            this, [this] (QAbstractSocket::SocketError err){emit sigErrorOccurred(this, err);},
            Qt::DirectConnection);

    connect(_sct, &QAbstractSocket::readyRead,
            this, [this] (){ emit sigReadyRead(this);},
            Qt::DirectConnection);
}

void AbstractNodeInfo::setIsLocal(bool isLocal) {
    _isLocal = isLocal;
}

NodeCoonectionStatus AbstractNodeInfo::status() const {
    return _status;
}

void AbstractNodeInfo::setStatus(const NodeCoonectionStatus &status) {
    _status = status;
}

bool AbstractNodeInfo::confirmData() const {
    return _status != NodeCoonectionStatus::NotConnected;
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
    return isValid() && _sct->state() == QAbstractSocket::ConnectedState;
}

QDataStream &AbstractNodeInfo::fromStream(QDataStream &stream) {
    stream >> _networkAddress;
    return stream;
}

QDataStream &AbstractNodeInfo::toStream(QDataStream &stream) const {
    stream << _networkAddress;
    return stream;
}

uint qHash(NodeCoonectionStatus status) {
    return static_cast<uint>(status);
}

}
