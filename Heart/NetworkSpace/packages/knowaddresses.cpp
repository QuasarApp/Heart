/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "knowaddresses.h"

#include <QDataStream>

namespace QH {
namespace PKG {

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

QSet<NodeId> KnowAddresses::knowAddresses() const {
    return _knowAddresses;
}

void KnowAddresses::setKnowAddresses(const QSet<NodeId> &knowAddresses) {
    _knowAddresses = knowAddresses;
}
}
}
