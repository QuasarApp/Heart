/*
 * Copyright (C) 2018-2022 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "distversion.h"
namespace QH {

unsigned short DistVersion::max() const {
    return _max;
}

void DistVersion::setMax(unsigned short newMax) {
    _max = newMax;
}

QDataStream &DistVersion::fromStream(QDataStream &stream) {
    stream >> _min;
    stream >> _max;
    return stream;
}

QDataStream &DistVersion::toStream(QDataStream &stream) const {
    stream << _min;
    stream << _max;
    return stream;
}

unsigned short DistVersion::min() const {
    return _min;
}

void DistVersion::setMin(unsigned short newMin) {
    _min = newMin;
}

}
