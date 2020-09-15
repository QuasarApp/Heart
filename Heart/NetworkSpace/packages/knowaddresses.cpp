#include "knowaddresses.h"

#include <QDataStream>

namespace NP {

KnowAddresses::KnowAddresses() {
    
}

KnowAddresses::KnowAddresses(const Package &pkg) {
    fromBytes(pkg.toBytes());
    
}

bool KnowAddresses::isValid() const {
    return AbstractData::isValid();
}

bool KnowAddresses::copyFrom(const AbstractData * other) {
    if (!AbstractData::copyFrom(other))
        return false;

    auto otherObject = dynamic_cast<const KnowAddresses*>(other);
    if (!otherObject)
        return false;

    this->_knowAddresses = otherObject->_knowAddresses;

    return true;
}

QDataStream &KnowAddresses::fromStream(QDataStream &stream) {
    AbstractData::fromStream(stream);

    stream >> _knowAddresses;

    return stream;
}

QDataStream &KnowAddresses::toStream(QDataStream &stream) const {
    AbstractData::toStream(stream);

    stream << _knowAddresses;

    return stream;
}

QSet<BaseId> KnowAddresses::knowAddresses() const {
    return _knowAddresses;
}

void KnowAddresses::setKnowAddresses(const QSet<BaseId> &knowAddresses) {
    _knowAddresses = knowAddresses;
}

}
