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
#include <QIODevice>

namespace QH {

bool operator ==(const PermisionData &left, const PermisionData &right) {
    return left._id == right._id && left._address == right._address;
}

PermisionData::PermisionData(const QVariant &subject, const DbAddress &objcet) {
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

bool PermisionData::isValid() const {
    return address().isValid() && _id.isValid();
}

bool PermisionData::equal(const AbstractKey *other) const {
    auto otherObject = dynamic_cast<const PermisionData*>(other);
    if (!otherObject)
        return false;

    return _address == otherObject->_address && _id == otherObject->_id;
}

QString PermisionData::toString() const {
    return QString("DBAddress: %0, Owner Id: %1").
            arg(_address.toString(), _id.toString());
}

const DbAddress& PermisionData::address() const {
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

const QVariant& PermisionData::id() const {
    return _id;
}

void PermisionData::setId(const QVariant &Id) {
    _id = Id;
}

}
