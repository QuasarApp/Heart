#include "dbaddress.h"
#include <QDataStream>
#include <QHash>


namespace NP {

qint64 qHash(const DbAddress &address) {
    qint64 res = address.id;
    res <<= 32;
    return res + qHash(address.table);
}

DbAddress::DbAddress(const QString &address, int id) {
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
}
