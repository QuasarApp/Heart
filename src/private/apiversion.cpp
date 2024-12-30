//#
//# Copyright (C) 2020-2025 QuasarApp.
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

    stream >> _apisVersions;
    stream >> _packagesVersions;
    return stream;
}

QDataStream &APIVersion::toStream(QDataStream &stream) const {
    stream << _apisVersions;
    stream << _packagesVersions;

    return stream;
}

VersionData APIVersion::apisVersions() const {
    return _apisVersions;
}

void APIVersion::setApisVersions(const VersionData &newApisVersions) {
    _apisVersions = newApisVersions;
}

PackagesVersionData APIVersion::packagesVersions() const {
    return _packagesVersions;
}

void APIVersion::setPackagesVersions(const PackagesVersionData &newPackagesVersions) {
    _packagesVersions = newPackagesVersions;
}

bool APIVersion::isValid() const {
    if (_apisVersions.isEmpty())
        return false;

    for (const auto & version: _apisVersions) {
        if (version.max() < version.min()) {
            return false;
        }
    }

    for (const auto & version: _packagesVersions) {
        if (version.max() < version.min()) {
            return false;
        }
    }

    return true;
}

}
}
