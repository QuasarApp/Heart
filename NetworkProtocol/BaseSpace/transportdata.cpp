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
    stream >> _targetAddress;
    QByteArray array;
    stream >> array;
    _data.fromBytes(array);

    return stream;
}

QDataStream &TransportData::toStream(QDataStream &stream) const {
    stream << _targetAddress;
    stream << _data.toBytes();

    return stream;
}

NodeId TransportData::targetAddress() const {
    return _targetAddress;
}

void TransportData::setTargetAddress(const NodeId &targetAddress) {
    _targetAddress = targetAddress;
}

bool TransportData::isValid() const {
    return  _data.isValid() && _targetAddress.isValid();
}
}
