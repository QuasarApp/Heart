/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "dbdatarequest.h"

#include <QDataStream>
namespace NP {

DBDataRequest::DBDataRequest() {
    INIT_COMMAND
}

DBDataRequest::DBDataRequest(DBDataRequestCmd cmd, const DbAddress &address):
    DBDataRequest()
{
    setCmd(cmd);
    setAddress(address);
}

QDataStream &DBDataRequest::fromStream(QDataStream &stream) {
    AbstractData::fromStream(stream);
    stream >> requestCmd;
    stream >> _address;

    return stream;
}

QDataStream &DBDataRequest::toStream(QDataStream &stream) const {
    AbstractData::toStream(stream);

    stream << requestCmd;
    stream << _address;

    return stream;
}

bool DBDataRequest::isValid() const {
    return AbstractData::isValid() && requestCmd && _address.isValid();
}

bool DBDataRequest::copyFrom(const AbstractData * other) {

    if (!AbstractData::copyFrom(other))
        return false;

    auto otherObject = dynamic_cast<const DBDataRequest*>(other);
    if (!otherObject)
        return false;

    this->requestCmd = otherObject->requestCmd;
    this->_address = otherObject->_address;

    return true;
}

}
