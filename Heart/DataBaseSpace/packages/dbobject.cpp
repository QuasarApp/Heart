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
#include <QSharedPointer>

namespace QH {

DBObject::DBObject(const QString &tableName) {
    clear();
    _dbId.setTable(tableName);
}

DBObject::~DBObject() {

}

QString DBObject::tableName() const {
    return _dbId.table();
}

PrepareResult DBObject::prepareSelectQuery(QSqlQuery &q) const {

    QString queryString = "SELECT * FROM %0 " + getWhereBlock();

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

uint DBObject::dbKey() const {
    return HASH_KEY(DbAddressKey(_dbId));
}

QPair<QString, QString> DBObject::altarnativeKey() const {
    return {};
}

DbAddress DBObject::dbAddress() const {
    return _dbId;
}

QSharedPointer<DBObject> DBObject::clone() const {
    return QSharedPointer<DBObject>(cloneRaw());
}

DBObject *DBObject::cloneRaw() const {
    auto cloneObject = createDBObject();
    if (!cloneObject->copyFrom(this)) {
        return nullptr;
    }

    cloneObject->init();

    return cloneObject;
}

QString DBObject::toString() const {
    return AbstractData::toString() +
            QString(" %0").arg(_dbId.toString());
}

QString DBObject::getWhereBlock() const {
    QString whereBlock = "WHERE ";

    if (getId().isValid()) {
        whereBlock += "id='" + getId().toBase64() + "'";
    } else {
        auto altKeys = altarnativeKey();
        if (altKeys.first.isEmpty()) {
            return {};
        }
        whereBlock +=  altKeys.first + "='" + altKeys.second + "'";

    }

    return whereBlock;
}

PrepareResult DBObject::prepareRemoveQuery(QSqlQuery &q) const {

    QString queryString = "DELETE FROM %0 " + getWhereBlock();

    queryString = queryString.arg(tableName());

    if (!q.prepare(queryString)) {
        return PrepareResult::Fail;
    }

    return PrepareResult::Success;
}

QDataStream &DBObject::fromStream(QDataStream &stream) {
    AbstractData::fromStream(stream);

    stream >> _dbId;

    return stream;
}

QDataStream &DBObject::toStream(QDataStream &stream) const {
    AbstractData::toStream(stream);

    stream << _dbId;

    return stream;
}

bool DBObject::init() {
    if (!AbstractData::init())
        return false;

    if (isBundle()) {
        return true;
    }

    _dbId.setId(generateId());
    return _dbId.isValid();
}

bool DBObject::isValid() const {
    return AbstractData::isValid() && _dbId.isValid();
}

bool DBObject::copyFrom(const AbstractData * other) {
    if (!AbstractData::copyFrom(other))
        return false;

    auto otherObject = dynamic_cast<const DBObject*>(other);
    if (!otherObject)
        return false;

    this->_dbId = otherObject->_dbId;

    return true;
}

BaseId DBObject::getId() const {
    return dbAddress().id();
}

void DBObject::setId(const BaseId& id) {
    _dbId.setId(id);
}

void DBObject::clear() {
    setId({});
}

}
