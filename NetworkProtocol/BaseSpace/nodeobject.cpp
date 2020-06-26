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

NodeObject::NodeObject(const DbId &id):
    NodeObject() {
    setId(id);
}

DBObject *NodeObject::factory() const {
    return new NodeObject;
}

bool NodeObject::prepareSaveQuery(QSqlQuery &q) const {
    QString queryString = "INSERT INTO %0(%1) VALUES (%2)";
    queryString = queryString.arg(tableName());
    queryString = queryString.arg("id, pubKey");

    QString values;

    values += "'" + getId().toBase64() + "', ";
    values += "'" + _publickKey + "'";

    queryString = queryString.arg(values);

    return q.prepare(queryString);
}

bool NodeObject::fromSqlRecord(const QSqlRecord &q) {
    if (!DBObject::fromSqlRecord(q)) {
        return false;
    }

    setPublickKey(q.value("pubKey").toByteArray());

    return isValid();
}

QByteArray NodeObject::publickKey() const {
    return _publickKey;
}

void NodeObject::setPublickKey(const QByteArray &publickKey) {
    _publickKey = publickKey;
}

QDataStream &NodeObject::fromStream(QDataStream &stream) {
    DBObject::fromStream(stream);

    stream >> _publickKey;

    return stream;
}

QDataStream &NodeObject::toStream(QDataStream &stream) const {
    DBObject::toStream(stream);

    stream << _publickKey;

    return stream;
}

bool NodeObject::isValid() const {
    return DBObject::isValid() && _publickKey.size() == 256;
}

bool NodeObject::copyFrom(const AbstractData * other) {
    if (!DBObject::copyFrom(other))
        return false;

    auto otherObject = dynamic_cast<const NodeObject*>(other);
    if (!otherObject)
        return false;

    this->_publickKey = otherObject->_publickKey;

    return true;
}

}
