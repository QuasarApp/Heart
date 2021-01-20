/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "getsinglevalue.h"

#include <QSqlQuery>

namespace QH {


namespace PKG {

GetSingleValue::GetSingleValue(const DbAddress& address, const QString& field):
    DBObject(address) {

    _field = field;
}

QVariant GetSingleValue::value() const {
    return _value;
}

DBObject *GetSingleValue::createDBObject() const {
    return create<GetSingleValue>(dbAddress(), _field);
}

PrepareResult GetSingleValue::prepareSelectQuery(QSqlQuery &q) const {
    QString queryString = "SELECT %0 FROM %1 WHERE id='%2'";

    queryString = queryString.arg(_field, tableName(), getId().toString());

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
    return "";
}

}
}
