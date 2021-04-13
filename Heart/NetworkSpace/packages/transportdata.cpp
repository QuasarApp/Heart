/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "transportdata.h"
#include <QCryptographicHash>
#include <QDataStream>
#include <QDateTime>
namespace QH {
namespace PKG {

TransportData::TransportData() {
    
}

TransportData::TransportData(const HostAddress &sender):
    TransportData() {

    addNodeToRoute(sender);
}

TransportData::TransportData(const Package &package):
    TransportData() {

    fromBytes(package.data);
}

void TransportData::completeRoute(bool fRouteIsComplete) {
    _fRouteIsComplete = fRouteIsComplete;
}

const Package &TransportData::data() const {
    return _data;
}

void TransportData::setData(const Package &data) {
    _data = data;

    QByteArray tmp = _data.data;
    quint64 time = QDateTime::currentMSecsSinceEpoch();
    tmp.insert(sizeof(time), reinterpret_cast<char*>(&time));

    _packageId.fromRaw(QCryptographicHash::hash(tmp, QCryptographicHash::Sha256));
}

bool TransportData::setData(const AbstractData &data) {
    Package pkg;
    if (!data.toPackage(pkg)) {
        return false;
    }

    setData(pkg);
    return true;
}

QDataStream &TransportData::fromStream(QDataStream &stream) {
    AbstractData::fromStream(stream);

    stream >> _targetAddress;
    stream >> _senderID;
    stream >> _packageId;
    stream >> _fRouteIsComplete;
    stream >> _route;

    QByteArray array;
    stream >> array;
    _data.fromBytes(array);

    return stream;
}

QDataStream &TransportData::toStream(QDataStream &stream) const {

    AbstractData::toStream(stream);

    stream << _targetAddress;
    stream << _senderID;
    stream << _packageId;
    stream << _fRouteIsComplete;
    stream << _route;

    stream << _data.toBytes();

    return stream;
}

const NodeId& TransportData::packageId() const {
    return _packageId;
}

const QList<HostAddress>& TransportData::route() const {
    return _route;
}

bool TransportData::setRoute(const QList<HostAddress> &route) {

    QSet<HostAddress> test;
    for (const auto& address: route) {
        if (test.contains(address))
            return false;

        test.insert(address);
    }

    _route = route;
    completeRoute(_route.size());

    return _route.size();
}

void TransportData::addNodeToRoute(const HostAddress &node) {
    _route.push_back(node);
}

bool TransportData::strip(const HostAddress &correntAddress,
                          const HostAddress &availabelTarget) {

    QList<HostAddress>::Iterator begin = _route.end();
    QList<HostAddress>::Iterator end = _route.end();

    for (auto it = _route.begin(); it != _route.end(); ++it) {
        if (correntAddress == *it) {
            begin = it;
        }

        if (availabelTarget == *it) {
            end = it;
            break;
        }
    }

    if (begin != _route.end() && end != _route.end()) {
        _route.erase(begin, end);
        return true;
    }

    return false;
}

const NodeId &TransportData::targetAddress() const {
    return _targetAddress;
}

void TransportData::setTargetAddress(const NodeId &targetAddress) {
    _targetAddress = targetAddress;
}

bool TransportData::isValid() const {
    return AbstractData::isValid() && _data.isValid() && _targetAddress.isValid();
}

bool TransportData::isHaveRoute() const {
    return isValid() && _route.size() && _fRouteIsComplete;
}
}
}
