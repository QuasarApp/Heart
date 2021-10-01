/*
 * Copyright (C) 2021-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "bigdatarequest.h"

namespace QH {
namespace PKG {

BigDataRequest::BigDataRequest() {

}

const QList<int> &BigDataRequest::neededParts() const {
    return _neededParts;
}

void BigDataRequest::setNeededParts(const QList<int> &newNeededParts) {
    _neededParts = newNeededParts;
}

bool BigDataRequest::isFinishedSuccessful() const {
    return _neededParts.isEmpty();
}

QDataStream &BigDataRequest::fromStream(QDataStream &stream) {
    BigDataBase::fromStream(stream);

    stream >> _neededParts;

    return stream;
}

QDataStream &BigDataRequest::toStream(QDataStream &stream) const {
    BigDataBase::toStream(stream);

    stream << _neededParts;

    return stream;
}

}
}
