/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "userrequest.h"

#include <QDataStream>

namespace NP {

UserRequest::UserRequest()
    : UserBaseData() {
    INIT_COMMAND

}

UserRequest::UserRequest(const Package &package):
    UserRequest() {
    fromBytes(package.data);
}

QDataStream &UserRequest::fromStream(QDataStream &stream) {
    UserBaseData::fromStream(stream);
    stream >> requestCmd;

    return stream;
}

QDataStream &UserRequest::toStream(QDataStream &stream) const {
    UserBaseData::toStream(stream);
    stream << requestCmd;

    return stream;
}

bool UserRequest::isValid() const {
    return  UserBaseData::isValid() &&
            requestCmd > static_cast<unsigned char>(UserRequestCmd::Invalid) &&
            requestCmd <= static_cast<unsigned char>(UserRequestCmd::Delete);
}

void UserRequest::clear() {
    UserBaseData::clear();
    requestCmd = static_cast<unsigned char>(UserRequestCmd::Invalid);
}
}
