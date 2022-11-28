/*
 * Copyright (C) 2018-2022 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "dbaddress.h"
#include <QDataStream>
#include <QHash>
#include <QCryptographicHash>


namespace QH {

qint64 qHash(const DbAddress &address) {
    return qHash(address.SHA256Hash());
}

DbAddress::DbAddress(const QString &table, const QVariant &id) {
    this->_table = table;
    this->_value = id;
    recalcHash();

}

bool operator==(const DbAddress & left, const DbAddress &other) {
    return left._table == other._table &&  left._value == other._value;
}

QDataStream &DbAddress::fromStream(QDataStream &stream) {
    stream >> _value;
    stream >> _table;
    recalcHash();

    return stream;
}

QDataStream &DbAddress::toStream(QDataStream &stream) const {
    stream << _value;
    stream << _table;
    return stream;
}

QString DbAddress::toString() const {
    return QString("DbAddress: table:%0, value:%1, Sha256:%2").
            arg(_table, _value.toString(), _SHA256Hash.toHex());
}

bool operator!=(const DbAddress &left, const DbAddress &other) {
    return !operator==(left, other);
}

bool DbAddress::isValid() const {
    return _value.isValid() && _table.size();
}

const QString& DbAddress::table() const {
    return _table;
}

void DbAddress::setTable(const QString &table) {
    _table = table;
    recalcHash();
}

const QVariant &DbAddress::id() const {
    return _value;
}

void DbAddress::setId(const QVariant &id){
    _value = id;
    recalcHash();
}

QByteArray DbAddress::SHA256Hash() const {
    return _SHA256Hash;
}

void DbAddress::recalcHash() {
    _SHA256Hash = QCryptographicHash::hash(toBytes(), QCryptographicHash::Sha256);
}

}
