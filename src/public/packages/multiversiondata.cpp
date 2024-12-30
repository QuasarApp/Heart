/*
 * Copyright (C) 2023-2025 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "multiversiondata.h"
#include "qaglobalutils.h"

#include <QIODevice>

namespace QH {
namespace PKG {

#define MAGIC "mver"

MultiversionData::MultiversionData(const QMap<unsigned short /*version*/, SerializationBox>& serializers):
    _serializers(serializers) {

    if (serializers.size()) {
        _packageVersion.setMax(serializers.lastKey());
        _packageVersion.setMin(serializers.firstKey());
    }
}

QDataStream &MultiversionData::fromStream(QDataStream &stream) {

    if (!_serializers.size()) {
        debug_assert(false,
                     "Your MultiversionData not support any serialized functions. "
                     "Please initialize it in constructor of the MultiversionData class.");
        return stream;
    }

    QByteArray magic;
    stream >> magic;

    unsigned short version = 0;
    if (magic == MAGIC) {
        stream >> version;
    } else {
        stream.device()->seek(0);
    }

    return _serializers.value(version).from(stream);
}

QDataStream &MultiversionData::toStream(QDataStream &stream) const {

    if (!_serializers.size()) {
        debug_assert(false,
                     "Your MultiversionData not support any serialized functions. "
                     "Please initialize it in constructor of the MultiversionData class.");
        return stream;
    }

    stream << QByteArray{MAGIC};
    stream << _serializers.lastKey();

    return _serializers.last().to(stream);
}

QByteArray MultiversionData::toBytesOf(const DistVersion& version) const {
    QByteArray res;
    QDataStream stream(&res, QIODevice::WriteOnly);

    if (parsingVersion()) {
        stream.setVersion(parsingVersion());
    }

    toStreamOf(stream, version);
    return res;
}

QDataStream &MultiversionData::toStreamOf(QDataStream &stream, const DistVersion& version) const {

    unsigned short ver = _packageVersion.getMaxCompatible(version);
    auto serializer = _serializers.value(ver, {});
    if (!serializer.to) {
        debug_assert(false,
                     "Your MultiversionData not support the required version serialized functions. "
                     "Please initialize it in constructor of the MultiversionData class.");
        return stream;
    }

    if (ver) {
        stream << QByteArray{MAGIC};
        stream << ver;
    }

    return serializer.to(stream);
}

bool MultiversionData::toPackage(Package &package,
                                 const DistVersion& reqVersion,
                                 unsigned int triggerHash) const {
    if (!isValid()) {
        return false;
    }

    package.data = toBytesOf(reqVersion);
    package.hdr.command = cmd();
    package.hdr.triggerHash = triggerHash;
    package.hdr.size = package.data.size();

    package.hdr.hash = package.calcHash();


    return package.isValid();
}

const DistVersion &MultiversionData::packageVersion() const {
    return _packageVersion;
}

}
}
