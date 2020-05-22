/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "abstractdata.h"
#include <QDataStream>
#include <QMap>
#include <typeinfo>
#include "networkprotocol.h"
#include <limits>
#include <quasarapp.h>

namespace NP {

unsigned short AbstractData::cmd() const {
    return _cmd;
}

void AbstractData::setCmd(unsigned short cmd) {
    _cmd = cmd;
}

AbstractData::AbstractData() {
    _cmd = 0;
}

bool AbstractData::fromBytes(const QByteArray &data) {
    return StreamBase::fromBytes(data);
}

QByteArray AbstractData::toBytes() const {
    return StreamBase::toBytes();
}

bool AbstractData::toPackage(Package &package,
                        unsigned short trigeredCommand) const {

    if (!isValid()) {
        return false;
    }

    package.data = toBytes();

    package.hdr.command = _cmd;
    package.hdr.triggerCommnad = trigeredCommand;
    package.hdr.size = static_cast<unsigned short>(package.data.size());

    return package.isValid();
}

QDataStream &AbstractData::fromStream(QDataStream &stream) {
    stream >> _cmd;
    return stream;
}

QDataStream &AbstractData::toStream(QDataStream &stream) const {
    stream << _cmd;
    return stream;
}

bool AbstractData::isValid() const {
    return _cmd;
}

bool AbstractData::copyFrom(const AbstractData *other) {
    return other;
}

AbstractData::~AbstractData() {

}


}
