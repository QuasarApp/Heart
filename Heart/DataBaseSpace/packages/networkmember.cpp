/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#include "networkmember.h"
#include <quasarapp.h>
#include <QSqlQuery>
#include <QSqlError>
#include <QDataStream>


namespace QH {
namespace PKG {

NetworkMember::NetworkMember():DBObject("NetworkMembers") {
    
}

NetworkMember::NetworkMember(const Package &pkg):
    NetworkMember() {
    fromBytes(pkg.data);
}

NetworkMember::NetworkMember(const QVariant &id):
    NetworkMember() {
    setId(id);
}

bool NetworkMember::fromSqlRecord(const QSqlRecord &q) {
    if (!DBObject::fromSqlRecord(q)) {
        return false;
    }

    setAuthenticationData(q.value("authenticationData").toByteArray());
    setTrust(q.value("trust").toInt());

    return NetworkMember::isValid();
}

QByteArray NetworkMember::authenticationData() const {
    return _authenticationData;
}

void NetworkMember::setAuthenticationData(const QByteArray &publickKey) {
    _authenticationData = publickKey;
}

QDataStream &NetworkMember::fromStream(QDataStream &stream) {
    DBObject::fromStream(stream);

    stream >> _authenticationData;
    stream >> _trust;

    return stream;
}

QDataStream &NetworkMember::toStream(QDataStream &stream) const {
    DBObject::toStream(stream);

    stream << _authenticationData;
    stream << _trust;
    return stream;
}

DBVariantMap NetworkMember::variantMap() const {

    auto map = DBObject::variantMap();
//    map[primaryKey()].type =        MemberType::Unique;
    map["authenticationData"] =     {_authenticationData,   MemberType::InsertUpdate};
    map["trust"] =                  {_trust,   MemberType::InsertUpdate};

    return  map;
}

QString NetworkMember::primaryKey() const {
    return "id";
}

int NetworkMember::trust() const {
    return _trust;
}

void NetworkMember::changeTrust(int diff) {
    _trust += diff;
}

void NetworkMember::setTrust(int trust) {
    _trust = trust;
}

bool NetworkMember::isValid() const {
    return DBObject::isValid();
}

bool NetworkMember::copyFrom(const AbstractData * other) {
    if (!DBObject::copyFrom(other))
        return false;

    auto otherObject = dynamic_cast<const NetworkMember*>(other);
    if (!otherObject)
        return false;

    this->_authenticationData = otherObject->_authenticationData;
    this->_trust = otherObject->_trust;

    return true;
}

}
}
