/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#include "nodeobject.h"
#include <QCryptographicHash>

namespace QH {
namespace PKG {

NodeObject::NodeObject() {

}

NodeObject::NodeObject(const Package &pkg):
    NetworkMember(pkg) {

}

QDataStream &NodeObject::fromStream(QDataStream &stream) {

    NetworkMember::fromStream(stream);

    stream >> _senderID;

    return stream;
}

QDataStream &NodeObject::toStream(QDataStream &stream) const {
    NetworkMember::toStream(stream);

    stream << _senderID;

    return stream;
}

bool NodeObject::init() {
    if (!NetworkMember::init())
        return false;

    if (authenticationData().isEmpty()) {
        return true;
    }

    setId(QCryptographicHash::hash(authenticationData(), QCryptographicHash::Sha256));

    return true;
}

DBObject *NodeObject::createDBObject() const {
    return create<NodeObject>();
}

bool NodeObject::copyFrom(const AbstractData *other) {
    if (!NetworkMember::copyFrom(other)) {
        return false;
    }

    auto otherObject = dynamic_cast<const NodeObject*>(other);
    if (!otherObject)
        return false;

    _senderID = otherObject->_senderID;

    return true;
}

}
}
