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
    this->_permisions = otherObject->_permisions;

    return true;
}

PrepareResult MemberPermisionObject::prepareSaveQuery(QSqlQuery &q) const {

    if (!isValid()) {
        return PrepareResult::Fail;
    }

    QString queryString = "INSERT INTO %0(%1) VALUES (%3) "
                          "ON CONFLICT(MemberPermisionsIndex) DO UPDATE SET %2";

    queryString = queryString.arg(tableName());

    queryString = queryString.arg(
                "memberId, objectTable", "objectId", "lvl");

    queryString = queryString.arg("memberId='" + _key.id().toBase64() +  "', " +
                                  "objectTable='" + _key.address().table()+  "', " +
                                  "objectId='" + _key.address().id().toBase64() +  "', " +
                                  "lvl='" + QString::number(static_cast<int>(_permisions)) + "'");
    QString values;

    values += "'" + _key.id().toBase64() + "', ";
    values += "'" + _key.address().table() + "', ";
    values += "'" + _key.address().id().toBase64() + "', ";
    values += QString::number(static_cast<int>(_permisions));



    queryString = queryString.arg(values);

    if (q.prepare(queryString))
        return PrepareResult::Success;
    return PrepareResult::Fail;
}

PrepareResult MemberPermisionObject::prepareRemoveQuery(QSqlQuery &q) const {
    if (!isValid()) {
        return PrepareResult::Fail;
    }

    QString queryString = "DELETE FROM %0 where memberId='%1' and objectTable='%2' and objectId='%3'";
    queryString = queryString.arg(tableName(),
                                  _key.id().toBase64(),
                                  _key.address().table(),
                                  _key.address().id().toBase64());

    if (q.prepare(queryString))
        return PrepareResult::Success;
    return PrepareResult::Fail;
}

PrepareResult MemberPermisionObject::prepareSelectQuery(QSqlQuery &q) const {
    if (_key.isValid()) {
        return PrepareResult::Fail;
    }

    QString queryString = "SELECT * FROM %0 WHERE";
    bool fOptionAdded = false;

    if (_key.id().isValid()) {
        queryString += "memberId='" + _key.id().toBase64() + "'";
        fOptionAdded = true;
    }

    if (_key.address().table().size()) {
        if (fOptionAdded)
            queryString += " and ";
        queryString += "objectTable='" + _key.address().table() + "'";
        fOptionAdded = true;
    }

    if (_key.address().id().isValid()) {
        if (fOptionAdded)
            queryString += " and ";
        queryString += "objectId='" + _key.address().id().toBase64() + "'";
        fOptionAdded = true;
    }

    queryString = queryString.arg(tableName());

    if (q.prepare(queryString))
        return PrepareResult::Success;
    return PrepareResult::Fail;
}

DBObject *MemberPermisionObject::createDBObject() const {
    return create<MemberPermisionObject>();
}

uint MemberPermisionObject::dbKey() const {
    return HASH_KEY(_key);
}

QDataStream &MemberPermisionObject::fromStream(QDataStream &stream) {

    stream >> _key;
    stream >> _permisions;

    return  stream;
}

QDataStream &MemberPermisionObject::toStream(QDataStream &stream) const {
    stream << _key;
    stream << _permisions;

    return  stream;
}

QVariant MemberPermisionObject::generateId() const {
    if (!_key.isValid())
        return {};

    return _key.hash();
}

DBVariantMap MemberPermisionObject::variantMap() const {
    return {};
}

PermisionData MemberPermisionObject::key() const {
    return _key;
}

void MemberPermisionObject::setKey(const PermisionData &key) {
    _key = key;
}

Permission MemberPermisionObject::permisions() const {
    return _permisions;
}

void MemberPermisionObject::setPermisions(const Permission &permisions) {
    _permisions = permisions;
}
}
}
