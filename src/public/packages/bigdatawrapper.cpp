
//#
//# Copyright (C) 2022-2025 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#include "bigdatawrapper.h"

namespace QH {
namespace PKG {
const AbstractData *BigDataWrapper::data() const {
    return _data;
}

void BigDataWrapper::setData(const AbstractData *newData) {
    _data = newData;
}

QString BigDataWrapper::toString() const {
    if (_data)
        return QString("Wrapper of : %0").arg(_data->toString());
    return QString("Wrapper of : null");
}

QDataStream &BigDataWrapper::fromStream(QDataStream &stream) {
    return stream;
}

QDataStream &BigDataWrapper::toStream(QDataStream &stream) const {
    return stream;
}

}
}
