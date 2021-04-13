/*
 * Copyright (C) 2018-2021 QuasarApp.
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
    return left._id == right._id && left._addressHash == right._addressHash;
}

PermisionData::PermisionData(const QVariant &subject, const DbAddress &objcet) {
    setId(subject);
    setAddress(objcet);
}

PermisionData::PermisionData(const QVariant &subject, const QString &objectAddress) {
    setId(subject);
    setAddress(objectAddress);
}

unsigned int PermisionData::hash() const {
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);

    stream << _id;
    stream << _addressHash;
    return qHash(data);
}

bool PermisionData::isValid() const {
    return _addressHash.size() && _id.isValid();
}

bool PermisionData::equal(const AbstractKey *other) const {
    auto otherObject = dynamic_cast<const PermisionData*>(other);
    if (!otherObject)
        return false;

    return _addressHash == otherObject->_addressHash && _id == otherObject->_id;
}

QString PermisionData::toString() const {
    return QString("DBAddressHash: %0, Owner Id: %1").
            arg(_addressHash, _id.toString());
}

const QString& PermisionData::addressHash() const {
    return _addressHash;
}

void PermisionData::setAddress(const DbAddress &address) {
    setAddress(address.SHA256Hash().toBase64(QByteArray::Base64UrlEncoding));
}

void PermisionData::setAddress(const QString &addressHash) {
    _addressHash = addressHash;
}

QDataStream &PermisionData::fromStream(QDataStream &stream) {
    stream >> _id;
    stream >> _addressHash;

    return stream;
}

QDataStream &PermisionData::toStream(QDataStream &stream) const {
    stream << _id;
    stream << _addressHash;

    return stream;
}

const QVariant& PermisionData::id() const {
    return _id;
}

void PermisionData::setId(const QVariant &Id) {
    _id = Id;
}

}
