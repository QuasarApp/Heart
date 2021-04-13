/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#include "longping.h"

namespace QH {
namespace PKG {

LongPing::LongPing(const NodeId &sender) {
    
    setSenderID(sender);
}

LongPing::LongPing(const Package &from) {
    fromBytes(from.data);
}

bool LongPing::isValid() const {
    return Ping::isValid() && _senderID.isValid();
}

bool LongPing::copyFrom(const AbstractData * other) {
    if (!Ping::copyFrom(other))
        return false;

    auto otherObject = dynamic_cast<const LongPing*>(other);
    if (!otherObject)
        return false;

    this->_senderID = otherObject->_senderID;
    return true;
}

QDataStream &LongPing::fromStream(QDataStream &stream) {
    Ping::fromStream(stream);
    stream >> _senderID;
    return stream;
}

QDataStream &LongPing::toStream(QDataStream &stream) const {
    Ping::toStream(stream);
    stream << _senderID;
    return stream;
}
}
}
