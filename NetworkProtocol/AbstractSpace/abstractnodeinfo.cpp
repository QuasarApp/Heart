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

namespace NP {

AbstractNodeInfo::AbstractNodeInfo(QAbstractSocket *sct) {
    setSct(sct);
}

AbstractNodeInfo::~AbstractNodeInfo() {}

QAbstractSocket *AbstractNodeInfo::sct() const {
    return _sct;
}

void AbstractNodeInfo::disconnect() {
    if (_sct) {
        _sct->close();
        _sct->deleteLater();
        _sct = nullptr;
    }
}

void AbstractNodeInfo::ban() {
    _trust = static_cast<int>(TrustNode::Baned);
    disconnect();
}

bool AbstractNodeInfo::isBaned() const {
    return _trust < 1;
}

void AbstractNodeInfo::unBan() {
    _trust = static_cast<int>(TrustNode::Restore);
}

void AbstractNodeInfo::setSct(QAbstractSocket *sct) {
    _sct = sct;
    if (_sct) {
        setNetworkAddress(HostAddress{_sct->peerAddress(), _sct->peerPort()});

        QHostInfo::lookupHost(networkAddress().toString(), [this] (QHostInfo info){
            if (dynamic_cast<AbstractNodeInfo*>(this)) {
                setInfo(info);
            }
        });
    }
}

HostAddress AbstractNodeInfo::networkAddress() const {
    if (_sct->isValid())
        return HostAddress{_sct->peerAddress(), _sct->peerPort()};

    return _networkAddress;
}

void AbstractNodeInfo::setNetworkAddress(const HostAddress &networkAddress) {
    _networkAddress = networkAddress;
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

    if (isBaned()) {
        disconnect();
    }
}

bool AbstractNodeInfo::isValid() const {
    return _sct;
}

bool AbstractNodeInfo::isConnected() const {
    return isValid() && _sct->isOpen();
}

QDataStream &AbstractNodeInfo::fromStream(QDataStream &stream) {
    stream >> _networkAddress;
    return stream;
}

QDataStream &AbstractNodeInfo::toStream(QDataStream &stream) const {
    stream << _networkAddress;
    return stream;
}

}
