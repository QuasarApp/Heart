/*
 * Copyright (C) 2018-2024 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "deleteobject.h"
namespace QH {
namespace PKG {
DeleteObject::DeleteObject() {

}

DeleteObject::DeleteObject(const DbAddress &address, const QString &primaryKey) {
    _primaryKey = primaryKey;
    _address = address;
}

DeleteObject::DeleteObject(const Package &pkg): DeleteObject() {
    fromBytes(pkg.toBytes());
}

DBObject *DeleteObject::createDBObject() const {
    return create<DeleteObject>();
}

DBVariantMap DeleteObject::variantMap() const {
    return {};
}

const AccessToken &DeleteObject::getSignToken() const {
    return _token;
}

void DeleteObject::setSignToken(const AccessToken &token) {
    _token = token;
}

bool DeleteObject::fromSqlRecord(const QSqlRecord &q) {
    Q_UNUSED(q);
    return true;
}

QDataStream &DeleteObject::fromStream(QDataStream &stream) {

    stream >> _address;
    stream >> _token;
    stream >> _primaryKey;

    return stream;
}

QDataStream &DeleteObject::toStream(QDataStream &stream) const {
    stream << _address;
    stream << _token;
    stream << _primaryKey;

    return stream;
}

void DeleteObject::setPrimaryKey(const QString &newPrimaryKey) {
    _primaryKey = newPrimaryKey;
}

bool DeleteObject::isCached() const {
    return false;
}

QString DeleteObject::table() const {
    return _address.table();
}

const DbAddress &DeleteObject::address() const {
    return _address;
}

void DeleteObject::setAddress(const DbAddress &newAddress) {
    _address = newAddress;
}

QString DeleteObject::primaryKey() const {
    return _primaryKey;
}

QVariant DeleteObject::primaryValue() const {
    return _address.id();
}

}
}
