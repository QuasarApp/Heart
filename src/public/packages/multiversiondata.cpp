/*
 * Copyright (C) 2023-2023 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "multiversiondata.h"
#include "qaglobalutils.h"

#include <QIODevice>

namespace QH {
namespace PKG {


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

    char* buf = new char[sizeof(magic)];
    stream.readRawData(buf, sizeof(magic)) ;

    unsigned short version = 0;
    if (buf == magic) {
        stream >> version;
    } else {
        stream.device()->seek(0);
    }

    delete[] buf;

    return _serializers.value(version).from(stream);
}

QDataStream &MultiversionData::toStream(QDataStream &stream) const {

    if (!_serializers.size()) {
        debug_assert(false,
                     "Your MultiversionData not support any serialized functions. "
                     "Please initialize it in constructor of the MultiversionData class.");
        return stream;
    }

    stream << magic;
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

    auto serializer = _serializers.value(_packageVersion.getMaxCompatible(version), {});
    if (!serializer.to) {
        debug_assert(false,
                     "Your MultiversionData not support the required version serialized functions. "
                     "Please initialize it in constructor of the MultiversionData class.");
        return stream;
    }

    stream << magic;
    stream << version;

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
