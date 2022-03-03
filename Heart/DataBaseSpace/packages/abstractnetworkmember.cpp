/*
 * Copyright (C) 2018-2022 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#include "abstractnetworkmember.h"
#include <quasarapp.h>
#include <QSqlQuery>
#include <QSqlError>
#include <QDataStream>


namespace QH {
namespace PKG {

AbstractNetworkMember::AbstractNetworkMember():DBObject("NetworkMembers") {
    
}

AbstractNetworkMember::AbstractNetworkMember(const Package &pkg):
    AbstractNetworkMember() {
    fromBytes(pkg.data);
}

AbstractNetworkMember::AbstractNetworkMember(const QVariant &id):
    AbstractNetworkMember() {
    setId(id);
}

bool AbstractNetworkMember::fromSqlRecord(const QSqlRecord &q) {
    if (!DBObject::fromSqlRecord(q)) {
        return false;
    }

    setAuthenticationData(q.value("authenticationData").toByteArray());
    setTrust(q.value("trust").toInt());

    return AbstractNetworkMember::isValid();
}

QByteArray AbstractNetworkMember::authenticationData() const {
    return _authenticationData;
}

void AbstractNetworkMember::setAuthenticationData(const QByteArray &publickKey) {
    _authenticationData = publickKey;
}

QDataStream &AbstractNetworkMember::fromStream(QDataStream &stream) {
    DBObject::fromStream(stream);

    stream >> _authenticationData;
    stream >> _trust;

    return stream;
}

QDataStream &AbstractNetworkMember::toStream(QDataStream &stream) const {
    DBObject::toStream(stream);

    stream << _authenticationData;
    stream << _trust;
    return stream;
}

DBVariantMap AbstractNetworkMember::variantMap() const {

    auto map = DBObject::variantMap();
    map["authenticationData"] =     {_authenticationData,   MemberType::InsertUpdate};
    map["trust"] =                  {_trust,                MemberType::InsertUpdate};

    return  map;
}

QString AbstractNetworkMember::primaryKey() const {
    return "id";
}

int AbstractNetworkMember::trust() const {
    return _trust;
}

void AbstractNetworkMember::changeTrust(int diff) {
    _trust += diff;
}

void AbstractNetworkMember::setTrust(int trust) {
    _trust = trust;
}

bool AbstractNetworkMember::isValid() const {
    return DBObject::isValid();
}

bool AbstractNetworkMember::copyFrom(const AbstractData * other) {
    if (!DBObject::copyFrom(other))
        return false;

    auto otherObject = dynamic_cast<const AbstractNetworkMember*>(other);
    if (!otherObject)
        return false;

    this->_authenticationData = otherObject->_authenticationData;
    this->_trust = otherObject->_trust;

    return true;
}

}
}
