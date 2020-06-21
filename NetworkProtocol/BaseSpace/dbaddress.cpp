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
    return qHash(address.id.toRaw() + address.table);
}

DbAddress::DbAddress(const QString &address, const DbId &id) {
    this->table = address;
    this->id = id;
}

bool operator==(const DbAddress & left, const DbAddress &other) {
    return left.table == other.table && left.id == other.id;
}

QDataStream &DbAddress::fromStream(QDataStream &stream) {
    stream >> id;
    stream >> table;
    return stream;
}

QDataStream &DbAddress::toStream(QDataStream &stream) const {
    stream << id;
    stream << table;
    return stream;
}

bool DbAddress::isValid() const {
    return id.isValid() && table.size();
}
}
