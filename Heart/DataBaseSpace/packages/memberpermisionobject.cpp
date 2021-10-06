/*
 * Copyright (C) 2018-2021 QuasarApp.
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
    MemberPermisionObject("MemberPermisions") {
    
}

MemberPermisionObject::MemberPermisionObject(const Package &pkg):
    MemberPermisionObject() {
    fromBytes(pkg.data);

}

MemberPermisionObject::MemberPermisionObject(const PermisionData &id):
    MemberPermisionObject()  {
    setKey(id);
}

MemberPermisionObject::MemberPermisionObject(const QString &tableName):
   DBObject(tableName) {

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

    PermisionData permision(q.value("memberId").toString(),
                            q.value("dbAddress").toString());

    setKey(permision);

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

DBVariantMap MemberPermisionObject::variantMap() const {
    return {{"memberId",               {_key.id(),                  MemberType::InsertUpdate}},
            {"dbAddress",              {_key.addressHash(),         MemberType::InsertUpdate}},
            {"lvl",                    {static_cast<unsigned char>(_permision), MemberType::InsertUpdate}}};
}

QString MemberPermisionObject::condition() const {
    QString result;
    if (_key.id().isValid()) {
        result += "memberId='" + _key.id().toString() + "'";
    }

    if (_key.addressHash().size()) {
        if (result.size()) {
            result += " AND ";
        }
        result += "dbAddress='" + _key.addressHash()+ "'";
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
    setId(_key.hash());
}

Permission MemberPermisionObject::permisions() const {
    return _permision;
}

void MemberPermisionObject::setPermisions(const Permission &permisions) {
    _permision = permisions;
}
}
}
