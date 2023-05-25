/*
 * Copyright (C) 2018-2023 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "abstractdata.h"
#include <QDataStream>
#include <QMap>
#include <typeinfo>
#include <limits>
#include <quasarapp.h>
#include <QCryptographicHash>
#include <qaglobalutils.h>

namespace QH {
namespace PKG {

AbstractData::AbstractData() {
}

bool AbstractData::toPackage(Package &package,
                             unsigned short reqVersion,
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

bool AbstractData::isValid() const {
    return true;
}

QString AbstractData::toString() const {
    return QString("Type: %0 \n"
                   "Version: %1 \n"
                   "Command: %2 \n").
        arg(cmdString(), packageVersion().toString()).
        arg(cmd());
}

void AbstractData::fromPakcage(const Package &pkg) {
    fromBytes(pkg.data);
}

QByteArray AbstractData::toBytesOf(unsigned short reqVersion) const {
    QByteArray res;
    QDataStream stream(&res, QIODevice::WriteOnly);

    if (parsingVersion()) {
        stream.setVersion(parsingVersion());
    }

    toStreamOf(stream, reqVersion);
    return res;
}

QDataStream &AbstractData::toStreamOf(QDataStream &stream, unsigned short version) const {
    debug_assert(packageVersion().max() == version, "from stream should be overload for the multi version packages.");

    return toStream(stream);
}

AbstractData::~AbstractData() {

}

QDataStream &AbstractData::fromStream(QDataStream &stream) {
    stream >> _packageVersion;
    return stream;
}

QDataStream &AbstractData::toStream(QDataStream &stream) const {
    stream << _packageVersion;
    return stream;
}

const DistVersion &AbstractData::packageVersion() const {
    return _packageVersion;
}

void AbstractData::setPackageVersion(const DistVersion &newPackageVersion) {
    _packageVersion = newPackageVersion;
}
}
}
