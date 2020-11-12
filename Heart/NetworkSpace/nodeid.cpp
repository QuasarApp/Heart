/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#include "nodeid.h"
namespace QH {

NodeId::NodeId() = default;


QH::NodeId::NodeId(const QVariant &val) {
    QVariant::Type type = val.type();
    if(type == QVariant::Type::ByteArray) {
        fromRaw(val.toByteArray());
    } else if (type == QVariant::Type::String) {
        fromBase64(val.toByteArray());
    } else {
        unsigned int integer = val.toUInt();
        fromRaw(reinterpret_cast<char*>(&integer), sizeof (integer));
    }
}

NodeId::NodeId(unsigned int val) {
    fromRaw(reinterpret_cast<char*>(&val), sizeof (val));
}

NodeId::NodeId(const QByteArray &raw) {
    fromRaw(raw);
}

NodeId::NodeId(const QString &base64) {
    fromBase64(base64.toLatin1());
}

bool NodeId::fromBase64(const QByteArray &base64) {
    return fromRaw(QByteArray::fromBase64(base64, QByteArray::Base64UrlEncoding));
}

bool NodeId::fromRaw(const QByteArray &raw) {
    _data = raw;
    return isValid();
}

bool NodeId::fromRaw(const char *data, int len) {
    _data.clear();
    _data.insert(0, data, len);
    return isValid();
}

QByteArray NodeId::toBase64() const {
    return _data.toBase64(QByteArray::Base64UrlEncoding);
}

const QByteArray& NodeId::toRaw() const {
    return _data;
}

bool NodeId::isValid() const {
    return _data.size();
}

void NodeId::clear() {
    _data.clear();
}

QDataStream &NodeId::fromStream(QDataStream &stream) {
    stream >> _data;
    return stream;
}

QDataStream &NodeId::toStream(QDataStream &stream) const {
    stream << _data;
    return stream;
}

bool operator==(const NodeId &left, const NodeId &other) {
    return left._data == other._data;
}


uint qHash(const QH::NodeId &object) {
    return qHash(object.toRaw());
}

}

