/*
 * Copyright (C) 2018-2022 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "setsinglevalue.h"
#include "quasarapp.h"
#include <QSqlError>
#include <QSqlQuery>

namespace QH {


namespace PKG {


SetSingleValue::SetSingleValue(const DbAddress& address,
                               const QString& field,
                               const QVariant& value,
                               const QString &primaryKey):
    DBObject(address.table())
{
    _id = address.id().toString();
    _field = field;
    _value = value;
    _primaryKey = primaryKey;
}

DBObject *SetSingleValue::createDBObject() const {
    return create<SetSingleValue>(dbAddress(), _field, _value);
}

PrepareResult SetSingleValue::prepareUpdateQuery(QSqlQuery &q) const {
    QString queryString = "UPDATE %0 SET %1=:%1 WHERE %2='%3'";

    queryString = queryString.arg(tableName(), _field, primaryKey(), _id);

    if (!q.prepare(queryString)) {

        QuasarAppUtils::Params::log("Failed to prepare query: " + q.lastError().text(),
                                    QuasarAppUtils::Error);
        return PrepareResult::Fail;
    }

    q.bindValue(":" + _field, _value);

    return PrepareResult::Success;
}

PrepareResult SetSingleValue::prepareInsertQuery(QSqlQuery &q) const {
    QString queryString = "INSERT INTO %0 (%1, %2) VALUES (:%1, :%2)";

    queryString = queryString.arg(tableName(), primaryKey(), _field);

    if (!q.prepare(queryString)) {

        QuasarAppUtils::Params::log("Failed to prepare query: " + q.lastError().text(),
                                    QuasarAppUtils::Error);
        return PrepareResult::Fail;
    }

    q.bindValue(":" + primaryKey(), primaryValue());
    q.bindValue(":" + _field, _value);

    return PrepareResult::Success;
}

bool SetSingleValue::fromSqlRecord(const QSqlRecord &) {
    return true;
}

bool SetSingleValue::isCached() const {
    return false;
}

QString SetSingleValue::primaryKey() const {
    return _primaryKey;
}

QString SetSingleValue::primaryValue() const {
    return _id;
}
}
}
