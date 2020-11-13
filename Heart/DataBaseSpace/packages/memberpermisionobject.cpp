/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "memberpermisionobject.h"

#include <QDataStream>
#include <QSqlQuery>
#include <dbcachekey.h>

namespace QH {
namespace PKG {

MemberPermisionObject::MemberPermisionObject():
    DBObject("MemberPermisions") {
    
}

MemberPermisionObject::MemberPermisionObject(const Package &pkg):
    MemberPermisionObject() {
    fromBytes(pkg.data);

}

MemberPermisionObject::MemberPermisionObject(const PermisionData &id):
    MemberPermisionObject()  {
    setKey(id);
}

bool MemberPermisionObject::isValid() const {
    return _key.isValid();
}

bool MemberPermisionObject::copyFrom(const AbstractData *other) {
    if (!DBObject::copyFrom(other))
        return false;

    auto otherObject = dynamic_cast<const MemberPermisionObject*>(other);
    if (!otherObject)
        return false;

    this->_key = otherObject->_key;
    this->_permision = otherObject->_permision;

    return true;
}

DBObject *MemberPermisionObject::createDBObject() const {
    return create<MemberPermisionObject>();
}

uint MemberPermisionObject::dbKey() const {
    return HASH_KEY(_key);
}

bool MemberPermisionObject::fromSqlRecord(const QSqlRecord &q) {

    DbAddress address;
    address.fromBytes(q.value("dbAddress").toByteArray());

    PermisionData permision(q.value("memberId"), address);

    setPermisions(static_cast<Permission>(q.value("lvl").toUInt()));

    return isValid();
}

bool MemberPermisionObject::isCached() const {
    return true;
}

QDataStream &MemberPermisionObject::fromStream(QDataStream &stream) {

    stream >> _key;
    stream >> _permision;

    return  stream;
}

QDataStream &MemberPermisionObject::toStream(QDataStream &stream) const {
    stream << _key;
    stream << _permision;

    return  stream;
}

QVariant MemberPermisionObject::generateId() const {
    if (!_key.isValid())
        return {};

    return _key.hash();
}

DBVariantMap MemberPermisionObject::variantMap() const {
    return {{"memberId",               {_key.id(),                  MemberType::InsertUpdate}},
            {"dbAddress",              {_key.address().toBytes(),   MemberType::InsertUpdate}},
            {"lvl",                    {static_cast<unsigned char>(_permision), MemberType::InsertUpdate}}};
}

QString MemberPermisionObject::condition() const {
    QString result;
    if (_key.id().isValid()) {
        result += "memberId=" + _key.id().toString();
    }

    if (_key.id().isValid()) {
        if (result.size()) {
            result += ", ";
        }
        result += "dbAddress=" + _key.address().toBytes();
    }

    return result;
}

QString MemberPermisionObject::primaryKey() const {
    return "";
}

PermisionData MemberPermisionObject::key() const {
    return _key;
}

void MemberPermisionObject::setKey(const PermisionData &key) {
    _key = key;
}

Permission MemberPermisionObject::permisions() const {
    return _permision;
}

void MemberPermisionObject::setPermisions(const Permission &permisions) {
    _permision = permisions;
}
}
}
