/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "dbaddress.h"
#include <QDataStream>
#include <QHash>


namespace NP {

qint64 qHash(const DbAddress &address) {
    return qHash(address.id().toRaw() + address.table());
}

DbAddress::DbAddress(const QString &address, const DbId &id) {
    this->_table = address;
    this->_id = id;
}

bool operator==(const DbAddress & left, const DbAddress &other) {
    return left._table == other._table && left._id == other._id;
}

QDataStream &DbAddress::fromStream(QDataStream &stream) {
    stream >> _id;
    stream >> _table;
    return stream;
}

QDataStream &DbAddress::toStream(QDataStream &stream) const {
    stream << _id;
    stream << _table;
    return stream;
}

bool DbAddress::isValid() const {
    return _id.isValid() && _table.size();
}

const QString& DbAddress::table() const {
    return _table;
}

void DbAddress::setTable(const QString &table) {
    _table = table;
}

const DbId& DbAddress::id() const {
    return _id;
}

void DbAddress::setId(const DbId &id) {
    _id = id;
}

}
