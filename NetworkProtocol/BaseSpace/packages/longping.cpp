#include "longping.h"

namespace NP {

LongPing::LongPing(const BaseId& sender) {
    
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

QDataStream &NP::LongPing::fromStream(QDataStream &stream) {
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
