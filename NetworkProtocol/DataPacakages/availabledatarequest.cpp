/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "availabledatarequest.h"
namespace NP {

AvailableDataRequest::AvailableDataRequest() {
    INIT_COMMAND
}


AvailableDataRequest::AvailableDataRequest(const Package &pkg):AvailableData(pkg) {
    INIT_COMMAND
}
AccessToken AvailableDataRequest::token() const
{
    return _token;
}

void AvailableDataRequest::setToken(const AccessToken &token)
{
    _token = token;
}

QDataStream &AvailableDataRequest::fromStream(QDataStream &stream) {
    AvailableData::fromStream(stream);

    stream >> _token;

    return stream;
}

QDataStream &AvailableDataRequest::toStream(QDataStream &stream) const {
    AvailableData::toStream(stream);

    stream << _token;

    return stream;
}

}
