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

    if (isOldPackage()) {
        package.hdr.hash = package.calcHashOld();
    } else {
        package.hdr.hash = package.calcHash();
    }

    return package.isValid();
}

bool AbstractData::isValid() const {
    return true;
}

QString AbstractData::toString() const {
    return QString("Type: %0 \n"
                   "Version: %2 \n"
                   "Command: %1 \n").
        arg(cmdString()).
        arg(cmd()).arg(ver());
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
    debug_assert(version == ver(), "from stream should be overload for the multi version packages.");

    return toStream(stream);
}

AbstractData::~AbstractData() {

}

bool QH::PKG::AbstractData::isOldPackage() const {
    return false;
}


}
}
