#include "ping.h"

#include <QDataStream>
namespace QH {
namespace PKG {

Ping::Ping() {
    
}

Ping::Ping(const Package &from): Ping() {
    fromBytes(from.data);
}

bool Ping::isValid() const {
    return AbstractData::isValid();
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
}
