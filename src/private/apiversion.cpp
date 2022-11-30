//#
//# Copyright (C) 2020-2022 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#

#include "apiversion.h"

namespace QH {
namespace PKG {


APIVersion::APIVersion() {

}

QDataStream &APIVersion::fromStream(QDataStream &stream) {

    stream >> _version;
    return stream;
}

QDataStream &APIVersion::toStream(QDataStream &stream) const {
    stream << _version;
    return stream;
}

const VersionData &APIVersion::version() const {
    return _version;
}

void APIVersion::setVersion(const VersionData &newVersion) {
    _version = newVersion;
}

bool APIVersion::isValid() const {
    if (_version.isEmpty())
        return false;

    for (const auto & version: _version) {
        if (version.max() < version.min()) {
            return false;
        }
    }

    return true;
}

}
}
