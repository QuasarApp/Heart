/*
 * Copyright (C) 2018-2022 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "getsinglevalue.h"

#include <QSqlQuery>

namespace QH {


namespace PKG {

GetSingleValue::GetSingleValue(const DbAddress& address,
                               const QString& field,
                               const QString& primaryKey):
    DBObject(address.table()) {

    _id = address.id().toString();
    _field = field;
    _key = primaryKey;
}

QVariant GetSingleValue::value() const {
    return _value;
}

DBObject *GetSingleValue::createDBObject() const {
    return create<GetSingleValue>(dbAddress(), _field);
}

PrepareResult GetSingleValue::prepareSelectQuery(QSqlQuery &q) const {
    QString queryString = "SELECT %0 FROM %1 WHERE %2='%3'";

    queryString = queryString.arg(_field, tableName(), _key, _id);

    if (!q.prepare(queryString)) {
        return PrepareResult::Fail;
    }

    return PrepareResult::Success;
}

bool GetSingleValue::fromSqlRecord(const QSqlRecord &q) {
    _value = q.value(0);

    return true;
}

bool GetSingleValue::isCached() const {
    return false;
}

QString GetSingleValue::primaryKey() const {
    return _id;
}

QString GetSingleValue::primaryValue() const {
    return _field;
}

}
}
