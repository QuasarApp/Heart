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

AbstractNetworkMember::AbstractNetworkMember(): {
    
}

AbstractNetworkMember::AbstractNetworkMember(const Package &pkg):
    AbstractNetworkMember() {
    fromBytes(pkg.data);
}

AbstractNetworkMember::AbstractNetworkMember(const QString& id):
    AbstractNetworkMember() {
    setId(id);
}

bool AbstractNetworkMember::fromSqlRecord(const QSqlRecord &q) {

    setId(q.value("id").toString());
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

    stream >> _id;
    stream >> _authenticationData;
    stream >> _trust;

    return stream;
}

QDataStream &AbstractNetworkMember::toStream(QDataStream &stream) const {
    stream << _id;
    stream << _authenticationData;
    stream << _trust;
    return stream;
}

DBVariantMap AbstractNetworkMember::variantMap() const {

    return {{"id",                  {_id,                 QH::PKG::MemberType::PrimaryKey}},
            {"authenticationData",  {_authenticationData, QH::PKG::MemberType::InsertUpdate}},
            {"trust",               {_trust,              QH::PKG::MemberType::InsertUpdate}},

    };
}

QString AbstractNetworkMember::primaryKey() const {
    return "id";
}

QString AbstractNetworkMember::primaryValue() const {
    return _id;
}

const QString &AbstractNetworkMember::getId() const {
    return _id;
}

void AbstractNetworkMember::setId(const QString &newId) {
    _id = newId;
}

QString AbstractNetworkMember::table() const {
    return "NetworkMembers";
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

    this->_id = otherObject->_id;
    this->_authenticationData = otherObject->_authenticationData;
    this->_trust = otherObject->_trust;

    return true;
}

}
}
