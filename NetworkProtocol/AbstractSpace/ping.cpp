#include "ping.h"

#include <QDataStream>
namespace NP {

Ping::Ping() {
    INIT_COMMAND
}

Ping::Ping(const Package &from): Ping() {
    fromBytes(from.data);
}

bool Ping::isValid() const {
    return AbstractData::isValid();
}

bool Ping::copyFrom(const AbstractData * other) {
    if (!AbstractData::copyFrom(other))
        return false;

    auto otherObject = dynamic_cast<const Ping*>(other);
    if (!otherObject)
        return false;

    this->_ansver = otherObject->_ansver;
    return true;
}

bool Ping::ansver() const {
    return _ansver;
}

void Ping::setAnsver(bool ansver) {
    _ansver = ansver;
}

QDataStream &Ping::fromStream(QDataStream &stream) {
    AbstractData::fromStream(stream);
    stream >> _ansver;
    return stream;

}

QDataStream &Ping::toStream(QDataStream &stream) const {
    AbstractData::toStream(stream);
    stream << _ansver;
    return stream;
}
}
