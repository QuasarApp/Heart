/*
 * Copyright (C) 2021-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "bigdatapart.h"
namespace QH {
namespace PKG {
BigDataPart::BigDataPart() {

}

int BigDataPart::getPakckageNumber() const {
    return _pakckageNumber;
}

void BigDataPart::setPakckageNumber(int newPakckageNumber) {
    _pakckageNumber = newPakckageNumber;
}

bool BigDataPart::isValid() const {
    return BigDataBase::isValid() && _pakckageNumber >= 0 && _data.size();
}

QString BigDataPart::toString() const {
    return BigDataBase::toString() +
            " packageNumber: " +
            QString::number(_pakckageNumber) +
            " raw data size : " + QString::number(_data.size());
}

QDataStream &BigDataPart::fromStream(QDataStream &stream) {
    BigDataBase::fromStream(stream);

    stream >> _pakckageNumber;
    stream >> _data;

    return stream;
}

QDataStream &BigDataPart::toStream(QDataStream &stream) const {
    BigDataBase::toStream(stream);

    stream << _pakckageNumber;
    stream << _data;

    return stream;
}

const QByteArray &BigDataPart::data() const {
    return _data;
}

void BigDataPart::setData(const QByteArray &newData) {
    _data = newData;
}

}
}
