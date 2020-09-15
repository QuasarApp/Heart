#include "nodeobject.h"
namespace NP {

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

DBObject *NodeObject::factory() const {
    return create<NodeObject>();
}
}
