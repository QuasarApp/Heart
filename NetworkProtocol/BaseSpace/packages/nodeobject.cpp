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
#include <QDataStream>
#include <QCryptographicHash>
#include <nodeid.h>


namespace NP {

NodeObject::NodeObject():DBObject("Nodes") {
    INIT_COMMAND
}

NodeObject::NodeObject(const Package &pkg):
    NodeObject() {
    fromBytes(pkg.data);
}

NodeObject::NodeObject(const BaseId &id):
    NodeObject() {
    setId(id);
}

DBObject *NodeObject::factory() const {
    return new NodeObject;
}

PrepareResult NodeObject::prepareSaveQuery(QSqlQuery &q) const {
    QString queryString = "INSERT INTO %0(%1) VALUES (%2)";
    queryString = queryString.arg(tableName());
    queryString = queryString.arg("id, pubKey, trust");

    QString values;

    values += "'" + nodeId().toBase64() + "', ";
    values += "'" + _publickKey.toBase64(QByteArray::Base64UrlEncoding) + "', ";
    values +=  QString::number(_trust);

    queryString = queryString.arg(values);

    if (q.prepare(queryString)) {
        return PrepareResult::Success;
    }

    QuasarAppUtils::Params::log("Query:" + queryString,
                                QuasarAppUtils::Error);

    return PrepareResult::Fail;
}

bool NodeObject::fromSqlRecord(const QSqlRecord &q) {
    if (!DBObject::fromSqlRecord(q)) {
        return false;
    }

    setPublickKey(QByteArray::fromBase64(q.value("pubKey").toByteArray(),
                                         QByteArray::Base64UrlEncoding));
    setTrust(q.value("trust").toInt());

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
    stream >> _trust;

    return stream;
}

QDataStream &NodeObject::toStream(QDataStream &stream) const {
    DBObject::toStream(stream);

    stream << _publickKey;
    stream << _trust;
    return stream;
}

int NodeObject::trust() const {
    return _trust;
}

void NodeObject::changeTrust(int diff) {
    _trust += diff;
}

void NodeObject::setTrust(int trust) {
    _trust = trust;
}

BaseId NodeObject::nodeId() const {
    return NodeId(QCryptographicHash::hash(publickKey(), QCryptographicHash::Sha256));
}

bool NodeObject::isValid() const {
    return DBObject::isValid() && _publickKey.size() == 512;
}

bool NodeObject::copyFrom(const AbstractData * other) {
    if (!DBObject::copyFrom(other))
        return false;

    auto otherObject = dynamic_cast<const NodeObject*>(other);
    if (!otherObject)
        return false;

    this->_publickKey = otherObject->_publickKey;
    this->_trust = otherObject->_trust;

    return true;
}

QPair<QString, QString> NodeObject::altarnativeKey() const {
    return {"pubKey", _publickKey.toBase64(QByteArray::Base64UrlEncoding)};
}

}
