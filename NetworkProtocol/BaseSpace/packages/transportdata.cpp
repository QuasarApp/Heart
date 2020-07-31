/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "transportdata.h"
namespace NP {

TransportData::TransportData() {
    INIT_COMMAND
}

TransportData::TransportData(const Package &package):TransportData() {
    fromBytes(package.data);
}

const Package &TransportData::data() const {
    return _data;
}

void TransportData::setData(const Package &data) {
    _data = data;
}

void TransportData::setData(const AbstractData &data) {
    Package pkg;
    data.toPackage(pkg);
    setData(pkg);
}

QDataStream &TransportData::fromStream(QDataStream &stream) {
    AbstractData::fromStream(stream);

    stream >> _targetAddress;
    stream >> _senderID;

    QByteArray array;
    stream >> array;
    _data.fromBytes(array);

    return stream;
}

QDataStream &TransportData::toStream(QDataStream &stream) const {

    AbstractData::toStream(stream);

    stream << _targetAddress;
    stream << _senderID;
    stream << _data.toBytes();

    return stream;
}

QList<HostAddress> TransportData::route() const {
    return _route;
}

void TransportData::setRoute(const QList<HostAddress> &route) {
    _route = route;
}

void TransportData::addNodeToRoute(const HostAddress &route) {
    _route.push_back(route);
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

BaseId TransportData::targetAddress() const {
    return _targetAddress;
}

void TransportData::setTargetAddress(const BaseId &targetAddress) {
    _targetAddress = targetAddress;
}

bool TransportData::isValid() const {
    return  _data.isValid() && _targetAddress.isValid();
}
}
