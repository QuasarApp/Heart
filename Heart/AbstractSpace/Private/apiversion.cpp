//#
//# Copyright (C) 2020-2022 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#

#include "apiversion.h"

namespace RC {

APIVersion::APIVersion() {

}

QDataStream &APIVersion::fromStream(QDataStream &stream) {

    stream >> _minimum;
    stream >> _maximum;
    stream >> _APINameKey;

    return stream;
}

QDataStream &APIVersion::toStream(QDataStream &stream) const {
    stream << _minimum;
    stream << _maximum;
    stream << _APINameKey;

    return stream;
}

unsigned int APIVersion::APINameKey() const {
    return _APINameKey;
}

void APIVersion::setAPINameKey(unsigned int newAPINameKey) {
    _APINameKey = newAPINameKey;
}

unsigned short APIVersion::maximum() const {
    return _maximum;
}

void APIVersion::setMaximum(unsigned short newMaximum) {
    _maximum = newMaximum;
}

unsigned short APIVersion::minimum() const {
    return _minimum;
}

void APIVersion::setMinimum(unsigned short newMinimum) {
    _minimum = newMinimum;
}

}
