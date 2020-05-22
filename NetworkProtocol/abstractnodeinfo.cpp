/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "abstractnodeinfo.h"
#include <QHostAddress>
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

QHostAddress AbstractNodeInfo::id() const {
    if (_sct)
        return (_sct->peerAddress());

    return _id;
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
        _id = _sct->peerAddress();

        QHostInfo::lookupHost(_id.toString(), [this] (QHostInfo info){
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

    if (isBaned()) {
        disconnect();
    }
}

bool AbstractNodeInfo::isValid() const {
    return _sct;
}

QDataStream &AbstractNodeInfo::fromStream(QDataStream &stream) {
    stream >> _id;
    return stream;
}

QDataStream &AbstractNodeInfo::toStream(QDataStream &stream) const {
    stream << id();
    return stream;
}

}
