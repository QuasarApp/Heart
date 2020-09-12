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

DbAddress::DbAddress(const QString &table, const BaseId &id) {
    this->_table = table;
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

QString DbAddress::toString() const {
    return QString("DbAddress: id:%0, table:%1").
            arg(QString(_id.toBase64())).
            arg(_table);
}

bool operator!=(const DbAddress &left, const DbAddress &other) {
    return !operator==(left, other);
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

const BaseId& DbAddress::id() const {
    return _id;
}

void DbAddress::setId(const BaseId &id) {
    _id = id;
}

}
