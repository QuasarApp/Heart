/*
 * Copyright (C) 2018-2024 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "abstractdata.h"
#include <QDataStream>
#include <QMap>
#include <quasarapp.h>
#include <QCryptographicHash>
#include <qaglobalutils.h>

namespace QH {
namespace PKG {

AbstractData::AbstractData() {
}

bool AbstractData::toPackage(Package &package,
                             const DistVersion& ,
                             unsigned int triggerHash) const {

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

bool AbstractData::isValid() const {
    return true;
}

QString AbstractData::toString() const {
    return QString("Type: %0 \n"
                   "Command: %1 \n").
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
