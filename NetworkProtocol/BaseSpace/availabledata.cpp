/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "availabledata.h"

#include <QDataStream>

namespace NP {

AvailableData::AvailableData(): DBObject("") {
    INIT_COMMAND
}

AvailableData::AvailableData(const Package &pkg):AvailableData() {
    fromBytes(pkg.toBytes());
}

QDataStream &AvailableData::fromStream(QDataStream &stream) {
    AbstractData::fromStream(stream);
    stream >> _data;
    return stream;
}

QDataStream &AvailableData::toStream(QDataStream &stream) const {
    AbstractData::toStream(stream);
    stream << _data;
    return stream;
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
    return new AvailableData;
}

PrepareResult AvailableData::prepareSelectQuery(QSqlQuery &) const {
    return PrepareResult::Disabled;
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

QList<DbId> AvailableData::data() const {
    return _data;
}

void AvailableData::setData(const QList<DbId> &data) {
    _data = data;
}

DbId& AvailableData::operator[]( int key) {
    return _data[key];
}

}

