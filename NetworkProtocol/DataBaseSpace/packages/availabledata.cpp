/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "availabledata.h"
#include "permisiondata.h"

#include <QDataStream>
#include <QSqlQuery>

namespace NP {

AvailableData::AvailableData():
    DBObject("NodesPermisions") {
    
}

AvailableData::AvailableData(const Package &pkg):AvailableData() {
    fromBytes(pkg.data);
}

QDataStream &AvailableData::fromStream(QDataStream &stream) {
    DBObject::fromStream(stream);
    stream >> _data;
    return stream;
}

QDataStream &AvailableData::toStream(QDataStream &stream) const {
    AbstractData::toStream(stream);
    stream << _data;
    return stream;
}

BaseId AvailableData::generateId() const {
    return {};
}

bool AvailableData::isValid() const {
    return AbstractData::isValid() && _data.size();
}

bool AvailableData::copyFrom(const AbstractData * other) {
    if (!DBObject::copyFrom(other))
        return false;

    auto otherObject = dynamic_cast<const AvailableData*>(other);
    if (!otherObject)
        return false;

    this->_data = otherObject->_data;
    return true;
}

void AvailableData::clear() {
    DBObject::clear();
    _data.clear();
}

DBObject *AvailableData::factory() const {
    return create<AvailableData>();
}

PrepareResult AvailableData::prepareSelectQuery(QSqlQuery &q) const {

    if (!getId().isValid()) {
        return PrepareResult::Fail;
    }

    QString queryString = "SELECT objectTable,objectId FROM %0 WHERE nodeId='" + getId().toBase64() + "'";
    queryString = queryString.arg(tableName());

    if (q.prepare(queryString))
        return PrepareResult::Success;

    return PrepareResult::Fail;

}

PrepareResult AvailableData::prepareSaveQuery(QSqlQuery &) const {
    return PrepareResult::Disabled;
}

PrepareResult AvailableData::prepareRemoveQuery(QSqlQuery &) const {
    return PrepareResult::Disabled;
}

bool AvailableData::isCached() const {
    return false;
}

bool AvailableData::isBundle() const {
    return true;
}

bool AvailableData::fromSqlRecord(const QSqlRecord &q) {
    if (q.contains("objectTable") && q.contains("objectId")) {

        DbAddress address{q.value("objectTable").toString(), q.value("objectId").toString()};
        _data.push_back(address);
        return true;
    }

    return false;

}

QList<DbAddress> AvailableData::data() const {
    return _data;
}

void AvailableData::setData(const QList<DbAddress> &data) {
    _data = data;
}

DbAddress& AvailableData::operator[]( int key) {
    return _data[key];
}

}

