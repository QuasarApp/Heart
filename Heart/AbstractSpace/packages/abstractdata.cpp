/*
 * Copyright (C) 2018-2022 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "abstractdata.h"
#include <QDataStream>
#include <QMap>
#include <typeinfo>
#include "heart.h"
#include <limits>
#include <quasarapp.h>
#include <QCryptographicHash>
#include <qaglobalutils.h>

namespace QH {
namespace PKG {

AbstractData::AbstractData() {
}

bool AbstractData::toPackage(Package &package,
                             unsigned int triggerHash) const {

    if (!checkCmd()) {
        QuasarAppUtils::Params::log("You try send pacakge without QH_PACKAGE macross. Please add QH_PACKAGE macros to this class.",
                                    QuasarAppUtils::Error);
        return false;
    }

    if (!isValid()) {
        return false;
    }

    package.data = toBytes();

    package.hdr.command = cmd();
    package.hdr.triggerHash = triggerHash;
    package.hdr.size = package.data.size();
    package.hdr.hash = package.calcHash();

    return package.isValid();
}

QDataStream &AbstractData::fromStream(QDataStream &stream) {
    return stream;
}

QDataStream &AbstractData::toStream(QDataStream &stream) const {
    return stream;
}

bool AbstractData::checkCmd() const {
    unsigned int code = typeid (*this).hash_code();
    return code == localCode(); \
}

bool AbstractData::isValid() const {
    return true;
}

bool AbstractData::copyFrom(const AbstractData *other) {
    return other;
}

QString AbstractData::toString() const {
    return QString("Object: type:%0, command:%1").
            arg(cmdString()).
            arg(cmd());
}

void AbstractData::fromPakcage(const Package &pkg) {
    fromBytes(pkg.data);
}

AbstractData::~AbstractData() {

}


}
}
