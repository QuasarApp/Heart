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
#include <quasarapp.h>

namespace QH {
namespace PKG {

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

    auto map = variantMap().keys();

    QString queryString = "SELECT id," + map.join(",") + " FROM %0 " + getWhereBlock();

    queryString = queryString.arg(tableName());

    if (!q.prepare(queryString)) {
        return PrepareResult::Fail;
    }

    return PrepareResult::Success;
}

bool DBObject::fromSqlRecord(const QSqlRecord &q) {

    if (q.contains("id")) {
        setId(q.value("id").toByteArray());
        return true;
    }

    return false;
}

PrepareResult DBObject::prepareInsertQuery(QSqlQuery &q) const {

    DBVariantMap map = variantMap();

    if (!map.size()) {
        QuasarAppUtils::Params::log("The variantMap method return an empty map.",
                                    QuasarAppUtils::Error);

        return PrepareResult::Fail;
    }

    QString queryString = "INSERT INTO %0(%1) VALUES (%2) ";


    queryString = queryString.arg(tableName());
    QString tableInsertHeader = "";
    QString tableInsertValues = "";

    for (auto it = map.begin(); it != map.end(); ++it) {

        tableInsertHeader += it.key();
        tableInsertValues += ":" + it.key();

        if (it + 1 != map.end()) {
            tableInsertHeader += ", ";
            tableInsertValues += ", ";
        }

    }

    queryString = queryString.arg(tableInsertHeader);
    queryString = queryString.arg(tableInsertValues);

    if (q.prepare(queryString)) {

        for (auto it = map.begin(); it != map.end(); ++it) {
            q.bindValue(":" + it.key(), it.value().value);
        }

        return PrepareResult::Success;
    }

    return PrepareResult::Fail;
}

PrepareResult DBObject::prepareUpdateQuery(QSqlQuery &q) const {

    if (!isHaveAPrimaryKey()) {

        QuasarAppUtils::Params::log("The databae object do not has a primary key. ",
                                    QuasarAppUtils::Error);
        return PrepareResult::Fail;
    }

    DBVariantMap map = variantMap();

    if (!map.size()) {
        QuasarAppUtils::Params::log("The variantMap method return an empty map.",
                                    QuasarAppUtils::Error);

        return PrepareResult::Fail;
    }

    QString queryString = "UPDATE %0 SET %1 WHERE %2";

    queryString = queryString.arg(tableName());
    QString tableUpdateValues = "";
    QString tableUpdateRules = QString("%0 = :%0").
            arg(primaryKey());

    for (auto it = map.begin(); it != map.end(); ++it) {
        if (!(it.value().type & MemberType::Update)) {
            continue;
        }

        if (tableUpdateValues.size()) {
            tableUpdateValues += ", ";
        }

        tableUpdateValues += QString("%0= :%0").arg(it.key());

    }

    queryString = queryString.arg(tableUpdateValues);
    queryString = queryString.arg(tableUpdateRules);

    if (q.prepare(queryString)) {

        for (auto it = map.begin(); it != map.end(); ++it) {
            if (it.value().type != MemberType::InsertUpdate) {
                continue;
            }

            q.bindValue(":" + it.key(), it.value().value);
        }

        return PrepareResult::Success;
    }

    return PrepareResult::Fail;
}

bool DBObject::isCached() const {
    return isHaveAPrimaryKey();
}

bool DBObject::isBundle() const {
    return false;
}

uint DBObject::dbKey() const {
    return HASH_KEY(DbAddressKey(_dbId));
}

QString DBObject::condition() const {
    return {primaryKey() + "= '" + primaryValue().toString() + "'"};
}

const QVariant &DBObject::primaryValue() const {
    return _dbId.id();
}

void DBObject::setDbAddress(const DbAddress &address) {
    _dbId = address;
}

const DbAddress &DBObject::dbAddress() const {
    return _dbId;
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
    auto con = condition();

    if (!con.size())
        return "";

    QString whereBlock = "WHERE " + con;

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

QVariant DBObject::generateId() const {
    if (isHaveAPrimaryKey())
        return {};

    return 0;
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

DBVariantMap DBObject::variantMap() const {
    if (isHaveAPrimaryKey()) {
        return {{primaryKey(), {_dbId.id(), MemberType::Insert}}};
    }

    return {};
}

bool DBObject::isValid() const {
    if (!AbstractData::isValid())
        return false;

    if (isHaveAPrimaryKey()) {
        return _dbId.isValid();
    }

    return _dbId.table().size();
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

bool DBObject::isHaveAPrimaryKey() const {
    return primaryKey().size();
}

const QVariant& DBObject::getId() const {
    return dbAddress().id();
}

void DBObject::setId(const QVariant& id) {
    _dbId.setId(id);
}

void DBObject::clear() {
    setId({});
}

DBVariant::DBVariant() {

}

DBVariant::DBVariant(const QVariant &value, MemberType type) {
    this->value = value;
    this->type = type;
}

}
}
