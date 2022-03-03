/*
 * Copyright (C) 2018-2022 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "authrequest.h"

#include <QDataStream>

namespace QH {


namespace PKG {
AuthRequest::AuthRequest() {

}

AuthRequest::AuthRequest(const Package &pkg) {
    fromBytes(pkg.data);
}

unsigned char AuthRequest::getRequestCmd() const {
    return static_cast<unsigned char>(_request);
}

QDataStream &AuthRequest::fromStream(QDataStream &stream) {
    UserMember::fromStream(stream);
    stream >> _request;
    return stream;
}

QDataStream &AuthRequest::toStream(QDataStream &stream) const {
    UserMember::toStream(stream);
    stream << _request;
    return stream;
}


void AuthRequest::setRequest(const UserRequestType &request) {
    _request = request;
}

}
}
