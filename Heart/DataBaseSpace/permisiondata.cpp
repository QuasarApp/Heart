/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#include "dbaddress.h"
#include "permisiondata.h"
#include <quasarapp.h>
#include <QDataStream>

namespace QH {

bool operator ==(const PermisionData &left, const PermisionData &right) {
    return left._id == right._id && left._address == right._address;
}

PermisionData::PermisionData(const BaseId &subject, const DbAddress &objcet) {
    setId(subject);
    setAddress(objcet);
}

unsigned int PermisionData::hash() const {
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);

    stream << _id;
    stream << _address;
    return qHash(data);
}

const BaseId *PermisionData::id() const {
    return &_id;
}

const QString *PermisionData::table() const {
    return &_address.table();
}

bool PermisionData::isValid() const {
    return address().isValid() && _id.isValid();
}

DbAddress PermisionData::address() const {
    return _address;
}

void PermisionData::setAddress(const DbAddress &address) {
    _address = address;
}

QDataStream &PermisionData::fromStream(QDataStream &stream) {
    stream >> _id;
    stream >> _address;

    return stream;
}

QDataStream &PermisionData::toStream(QDataStream &stream) const {
    stream << _id;
    stream << _address;

    return stream;
}

void PermisionData::setId(const BaseId &Id) {
    _id = Id;
}

}
