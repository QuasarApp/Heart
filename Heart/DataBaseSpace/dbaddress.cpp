/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "dbaddress.h"
#include <QDataStream>
#include <QHash>


namespace QH {

qint64 qHash(const DbAddress &address) {
    return qHash(address.table() + address.id().toString());
}

DbAddress::DbAddress(const QString &table, const QVariant &id) {
    this->_table = table;
    this->_value = id;
}

bool operator==(const DbAddress & left, const DbAddress &other) {
    return left._table == other._table &&  left._value == other._value;
}

QDataStream &DbAddress::fromStream(QDataStream &stream) {
    stream >> _value;
    stream >> _table;
    return stream;
}

QDataStream &DbAddress::toStream(QDataStream &stream) const {
    stream << _value;
    stream << _table;
    return stream;
}

QString DbAddress::toString() const {
    return QString("DbAddress: table:%0, value:%1").
            arg(_table).arg(_value.toString());
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
}

const QVariant &DbAddress::id() const {
    return _value;
}

void DbAddress::setId(const QVariant &id){
    _value = id;
}

}
