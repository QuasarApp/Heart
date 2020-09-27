/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#include "nodeobject.h"
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

DBObject *NodeObject::createDBObject() const {
    return create<NodeObject>();
}
}
}
