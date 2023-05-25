/*
 * Copyright (C) 2023-2023 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "multiversiondata.h"

namespace QH {
namespace PKG {

MultiversionData::MultiversionData(const QMap<unsigned short /*version*/, SerializationBox>& serializers):
    _serializers(serializers) {}

QDataStream &MultiversionData::fromStream(QDataStream &stream) {
    AbstractData::fromStream(stream);

    packageVersion();

    return stream;
}

QDataStream &MultiversionData::toStream(QDataStream &stream) const {
    AbstractData::toStream(stream);

    return stream;
}

QDataStream &MultiversionData::toStreamOf(QDataStream &stream, unsigned short version) const {

}
}
}
