/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#include "nodeobject.h"
#include <quasarapp.h>
#include <QSqlQuery>
#include <QSqlError>


namespace NP {

NodeObject::NodeObject():DBObject("Nodes") {

}

NodeObject::NodeObject(const Package &pkg):
    NodeObject() {
    fromBytes(pkg.toBytes());
}

NodeObject::NodeObject(const QByteArray &id):
    NodeObject() {
    setId(id);
}

DBObject *NodeObject::factory() {
    return new NodeObject;
}

bool NodeObject::select(QSqlQuery &q) {
    QString query;

    if (getId().isValid()) {
        query = "SELECT * from '" + tableName() +
        "' where id='" +getId().toBase64() + "'";
    } else {
        query = "SELECT * from '" + tableName() +
        "' where pubKey='" + _publickKey + "'";
    }

    if (!q.prepare(query))
        return false;

    if (!q.exec())
        return false;

    if (!q.next())
        return false;

    setId(q.value("id").toString());
    _publickKey = q.value("pubKey").toByteArray();

    return isValid();
}

bool NodeObject::save(QSqlQuery &q) const {
    QString queryString = "INSERT INTO %0(%1) VALUES (%2)";
    queryString = queryString.arg(tableName());
    queryString = queryString.arg("id, pubKey");

    QString values;

    values += "'" + getId().toBase64() + "', ";
    values += "'" + _publickKey + "'";

    queryString = queryString.arg(values);

    if (!q.prepare(queryString)) {
        QuasarAppUtils::Params::log("prepare sql query error: " + q.lastError().text() + " Driver: " + q.lastError().driverText(),
                                    QuasarAppUtils::Debug);
        return false;
    }

    if (!q.exec()) {
        QuasarAppUtils::Params::log("exec sql query error: " + q.lastError().text() + " Driver: " + q.lastError().driverText(),
                                    QuasarAppUtils::Debug);
        return false;
    }

    return true;
}

}
