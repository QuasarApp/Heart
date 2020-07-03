/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "dbaddresskey.h"
#include "dbcachekey.h"
#include "dbobject.h"
#include <QDataStream>
#include <QDateTime>
#include <QSqlQuery>
#include <QHash>
#include <QSqlRecord>
#include <QVariantMap>

namespace NP {

DBObject::DBObject(const QString &tableName) {
    clear();
    _tableName = tableName;
}

DBObject::~DBObject() {

}

QString DBObject::tableName() const {
    return _tableName;
}

void DBObject::setTableName(const QString &tableName) {
    _tableName = tableName;
}

PrepareResult DBObject::prepareSelectQuery(QSqlQuery &q) const {
    if (_id.isValid()) {
        return PrepareResult::Fail;
    }

    QString queryString = "SELECT * FROM %0 WHERE id='" + getId().toBase64() + "'";

    queryString = queryString.arg(tableName());

    if (!q.prepare(queryString)) {
        return PrepareResult::Fail;
    }

    return PrepareResult::Success;
}

bool DBObject::fromSqlRecord(const QSqlRecord &q) {
    if (q.contains("id")) {
        setId(q.value("id").toString());
        return true;
    }

    return false;
}

bool DBObject::isCached() const {
    return true;
}

bool DBObject::isBundle() const {
    return false;
}

DBCacheKey DBObject::dbKey() const {
    return DBCacheKey::create<DbAddressKey>(DbAddress{tableName(), getId()});
}

DbAddress DBObject::dbAddress() const {
    return DbAddress{tableName(), getId()};
}

PrepareResult DBObject::prepareRemoveQuery(QSqlQuery &q) const {
    if (_id.isValid()) {
        return PrepareResult::Fail;
    }
    
    QString queryString = "DELETE FROM %0 where id=" + getId().toBase64();

    queryString = queryString.arg(tableName());

    if (!q.prepare(queryString)) {
        return PrepareResult::Fail;
    }

    return PrepareResult::Success;
}

QDataStream &DBObject::fromStream(QDataStream &stream) {
    AbstractData::fromStream(stream);

    stream >> _tableName;
    stream >> _id;

    BaseId senderNode;
    stream >> senderNode;
    setSenderID(senderNode);

    return stream;
}

QDataStream &DBObject::toStream(QDataStream &stream) const {
    AbstractData::toStream(stream);

    stream << _tableName;
    stream << _id;
    stream << senderID();

    return stream;
}

bool DBObject::isValid() const {
    return AbstractData::isValid() && _tableName.size();
}

bool DBObject::copyFrom(const AbstractData * other) {
    if (!AbstractData::copyFrom(other))
        return false;

    auto otherObject = dynamic_cast<const DBObject*>(other);
    if (!otherObject)
        return false;

    this->_tableName = otherObject->_tableName;
    this->_id = otherObject->_id;

    return true;
}

BaseId DBObject::getId() const {
    return _id;
}

void DBObject::setId(const BaseId& id) {
    _id = id;
}

void DBObject::clear() {
    _id.clear();
    setNodeId({});
}

}
