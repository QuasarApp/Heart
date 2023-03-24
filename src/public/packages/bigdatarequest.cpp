/*
 * Copyright (C) 2021-2023 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "bigdatarequest.h"

namespace QH {
namespace PKG {

BigDataRequest::BigDataRequest() {

}

QDataStream &BigDataRequest::fromStream(QDataStream &stream) {
    BigDataBase::fromStream(stream);

    stream >> _currentPart;

    return stream;
}

QDataStream &BigDataRequest::toStream(QDataStream &stream) const {
    BigDataBase::toStream(stream);

    stream << _currentPart;

    return stream;
}

int BigDataRequest::currentPart() const {
    return _currentPart;
}

void BigDataRequest::setCurrentPart(int newCurrentPart) {
    _currentPart = newCurrentPart;
}

}
}
