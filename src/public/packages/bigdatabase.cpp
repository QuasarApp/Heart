/*
 * Copyright (C) 2021-2023 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#include "bigdatabase.h"

namespace QH {
namespace PKG {

BigDataBase::BigDataBase()
{

}

int BigDataBase::packageId() const {
    return _packageId;
}

void BigDataBase::setPackageId(int newPackageId) {
    _packageId = newPackageId;
}

QDataStream &BigDataBase::fromStream(QDataStream &stream) {

    stream >> _packageId;

    return stream;
}

QDataStream &BigDataBase::toStream(QDataStream &stream) const {
    stream << _packageId;

    return stream;
}

bool BigDataBase::isValid() const {
    return AbstractData::isValid() && _packageId >= 0;

}

QString BigDataBase::toString() const {
    return AbstractData::toString() +
            " packageId: " +
            QString::number(_packageId);
}

}
}
