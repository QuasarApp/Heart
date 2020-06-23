/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "dbdatarequest.h"

#include <QDataStream>
namespace NP {

DBDataRequest::DBDataRequest(const Package &pkg):
    DBObject("") {
    fromBytes(pkg.toBytes());
}

DBDataRequest::DBDataRequest(const DbAddress &address):
    DBObject(address.table()) {

    INIT_COMMAND
    setId(address.id());
}

QDataStream &DBDataRequest::fromStream(QDataStream &stream) {
    AbstractData::fromStream(stream);
    return stream;
}

QDataStream &DBDataRequest::toStream(QDataStream &stream) const {
    AbstractData::toStream(stream);
    return stream;
}

bool DBDataRequest::isValid() const {
    return DBObject::isValid();
}

bool DBDataRequest::copyFrom(const AbstractData * other) {

    if (!DBObject::copyFrom(other))
        return false;

    auto otherObject = dynamic_cast<const DBDataRequest*>(other);
    if (!otherObject)
        return false;

    return true;
}

}
