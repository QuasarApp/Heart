
//#
//# Copyright (C) 2022-2025 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#include "bigdatawraper.h"

namespace QH {
namespace PKG {
const AbstractData *BigDataWraper::data() const {
    return _data;
}

void BigDataWraper::setData(const AbstractData *newData) {
    _data = newData;
}

QString BigDataWraper::toString() const {
    if (_data)
        return QString("Wrapper of : %0").arg(_data->toString());
    return QString("Wrapper of : null");
}

QDataStream &BigDataWraper::fromStream(QDataStream &stream) {
    return stream;
}

QDataStream &BigDataWraper::toStream(QDataStream &stream) const {
    return stream;
}

}
}
