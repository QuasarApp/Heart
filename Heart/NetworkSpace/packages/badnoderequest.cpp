/*
 * Copyright (C) 2018-2022 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "badnoderequest.h"
namespace QH {
namespace PKG {

BadNodeRequest::BadNodeRequest() {
    
}

BadNodeRequest::BadNodeRequest(const ErrorData &err):BadRequest(err) {
    
}

BadNodeRequest::BadNodeRequest(const Package &package): BadRequest(package) {
    
}

bool BadNodeRequest::isValid() const {
    return BadRequest::isValid() && senderID().isValid();
}

bool BadNodeRequest::copyFrom(const AbstractData * other) {
    if (!BadRequest::copyFrom(other))
        return false;

    auto otherObject = dynamic_cast<const BadNodeRequest*>(other);
    if (!otherObject)
        return false;

    setSenderID(otherObject->senderID());
    return true;
}

QDataStream &BadNodeRequest::fromStream(QDataStream &stream) {
    BadRequest::fromStream(stream);

    NodeId senderNode;
    stream >> senderNode;
    setSenderID(senderNode);

    return stream;
}

QDataStream &BadNodeRequest::toStream(QDataStream &stream) const {
    BadRequest::toStream(stream);
    stream << senderID();

    return stream;
}
}
}
