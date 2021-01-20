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
    DBObject::clear();
    _dbId.setTable(tableName);
}

DBObject::DBObject(const DbAddress &address) {
    _dbId = address;
}

DBObject::~DBObject() {

}

QString DBObject::tableName() const {
    return _dbId.table();
}

PrepareResult DBObject::prepareSelectQuery(QSqlQuery &q) const {

    auto map = variantMap().keys();

    QString queryString = "SELECT " + map.join(",") + " FROM %0 " + getWhereBlock();

    queryString = queryString.arg(tableName());

    if (!q.prepare(queryString)) {
        return PrepareResult::Fail;
    }

    return PrepareResult::Success;
}

bool DBObject::fromSqlRecord(const QSqlRecord &q) {

    QString key = primaryKey();
    if (key.size() && q.contains(key)) {
        setId(q.value(key));
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

        if (!bool(it.value().type & MemberType::Insert)) {
            continue;
        }

        tableInsertHeader += it.key();
        tableInsertValues += ":" + it.key();
        if (std::next(it, 1) != map.end()) {
            tableInsertHeader += ", ";
            tableInsertValues += ", ";
        }

    }

    queryString = queryString.arg(tableInsertHeader);
    queryString = queryString.arg(tableInsertValues);

    if (q.prepare(queryString)) {

        for (auto it = map.begin(); it != map.end(); ++it) {
            if (!bool(it.value().type & MemberType::Insert)) {
                continue;
            }

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

    QString queryString = "UPDATE %0 SET %1 WHERE " + DBObject::condition();

    queryString = queryString.arg(tableName());
    QString tableUpdateValues = "";

    for (auto it = map.begin(); it != map.end(); ++it) {
        if (!bool(it.value().type & MemberType::Update)) {
            continue;
        }

        if (tableUpdateValues.size()) {
            tableUpdateValues += ", ";
        }

        tableUpdateValues += QString("%0= :%0").arg(it.key());

    }

    queryString = queryString.arg(tableUpdateValues);

    if (q.prepare(queryString)) {

        for (auto it = map.begin(); it != map.end(); ++it) {
            if (!bool(it.value().type & MemberType::Update)) {
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

    // prepare key value to string condition
    auto prepareCondition = [](const QString& key, const QString &val){
        return key + "= '" + val + "'";
    };

    auto byteArrayWarning = [](){
        QuasarAppUtils::Params::log("You try generate a condition from the raw bytes array."
                                    " This operation can not be brok the sql request."
                                    " Use the QString or int type for values of condition."
                                    " If you want to  bytes array in condition then override the DBObject::condition method.",
                                    QuasarAppUtils::Warning);
    };

    QString errorString = "WRONG OBJECT";

    // if object have a primaryKey then return primary key
    auto primaryVal = primaryValue();
    if (primaryVal.isValid()) {

        if (primaryVal.type() == QVariant::ByteArray) {
            byteArrayWarning();
            return errorString;
        }

        return prepareCondition(primaryKey(), primaryValue().toString());
    }

    auto map = variantMap();

    // check all objects fields
    for (auto it = map.begin(); it != map.end(); ++it) {
        // if field if unique then to
        if (bool(it.value().type & MemberType::Unique)) {
            QVariant::Type type = it.value().value.type();

            // if field is string then check size.
            if (type == QVariant::String) {
                QString val = it.value().value.toString();
                if (val.size()) {
                    return prepareCondition(it.key(), val);
                }
            } else if (type == QVariant::ByteArray) {
                byteArrayWarning();
                continue;
            } else if (it.value().value.isValid()) {
                return prepareCondition(it.key(), it.value().value.toString());
            }
        }
    }

    QuasarAppUtils::Params::log("Fail to generate condition for object: " + toString() +
                                ". Object do not have valid unique fields or valid database address.",
                                QuasarAppUtils::Error);


    return errorString;
}

const QVariant &DBObject::primaryValue() const {
    return _dbId.id();
}

void DBObject::setDbAddress(const DbAddress &address) {
    _dbId = address;
}

bool DBObject::isInsertPrimaryKey() const {
    return bool(variantMap().value(primaryKey()).type & MemberType::Insert);
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

DBVariantMap DBObject::variantMap() const {
    if (isHaveAPrimaryKey()) {
        return {{primaryKey(), {_dbId.id(), MemberType::PrimaryKey}}};
    }

    return {};
}

bool DBObject::isValid() const {
    if (!AbstractData::isValid())
        return false;

    if (isInsertPrimaryKey()) {
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
