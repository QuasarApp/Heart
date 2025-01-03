/*
 * Copyright (C) 2018-2025 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "setsinglevalue.h"
#include "params.h"
#include <QSqlError>
#include <QSqlQuery>

namespace QH {


namespace PKG {


SetSingleValue::SetSingleValue(const DbAddress& address,
                               const QString& field,
                               const QVariant& value,
                               const QString &primaryKey) {
    _primaryValue = address.id();
    _table = address.table();
    _field = field;
    _value = value;
    _primaryKey = primaryKey;
}

DBObject *SetSingleValue::createDBObject() const {
    return create<SetSingleValue>(dbAddress(), _field, _value);
}

PrepareResult SetSingleValue::prepareUpdateQuery(QSqlQuery &q) const {
    QString queryString = "UPDATE %0 SET %1=:%1 WHERE %2=:%2";

    queryString = queryString.arg(table(), _field, primaryKey());

    if (!q.prepare(queryString)) {

        qCritical() << "Failed to prepare query: " + q.lastError().text();
        return PrepareResult::Fail;
    }

    q.bindValue(":" + _field, _value);
    q.bindValue(":" + _primaryKey, _primaryValue);

    return PrepareResult::Success;
}

PrepareResult SetSingleValue::prepareInsertQuery(QSqlQuery &q, bool replace) const {

    QString queryString = (replace)?
                              "REPLACE INTO %0 (%1, %2) VALUES (:%1, :%2)":
                              "INSERT INTO %0 (%1, %2) VALUES (:%1, :%2)";

    queryString = queryString.arg(table(), primaryKey(), _field);

    if (!q.prepare(queryString)) {

        qCritical() << "Failed to prepare query: " + q.lastError().text();
        return PrepareResult::Fail;
    }

    q.bindValue(":" + _primaryKey, _primaryValue);
    q.bindValue(":" + _field, _value);

    return PrepareResult::Success;
}

bool SetSingleValue::fromSqlRecord(const QSqlRecord &) {
    return true;
}

bool SetSingleValue::isCached() const {
    return false;
}

QString SetSingleValue::table() const {
    return _table;
}

QString SetSingleValue::primaryKey() const {
    return _primaryKey;
}

QVariant SetSingleValue::primaryValue() const {
    return _primaryValue;
}
}
}
