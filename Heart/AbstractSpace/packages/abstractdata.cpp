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
#include "heart.h"
#include <limits>
#include <quasarapp.h>
#include <QCryptographicHash>

namespace QH {
namespace PKG {
unsigned short AbstractData::cmd() const {
    return _cmd;
}

void AbstractData::setCmd(unsigned short cmd) {
    _cmd = cmd;
}

bool AbstractData::init() {
    if (typeid (*this).hash_code() == typeid(AbstractData).hash_code())
        return false;

    generateCmd();

    return true;
}

void AbstractData::generateCmd() {
    setCmd(H_16(*this));
}

AbstractData::AbstractData() {
    _cmd = 0;
}

bool AbstractData::toPackage(Package &package,
                             unsigned int triggerHash) const {

    if (!isValid()) {
        return false;
    }

    package.data = toBytes();

    package.hdr.command = _cmd;
    package.hdr.triggerHash = triggerHash;
    package.hdr.size = static_cast<unsigned short>(package.data.size());
    package.hdr.hash = qHash(package.data);

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

QString AbstractData::toString() const {
    return QString("Object: type:%0, command:%1").
            arg(typeid(*this).name()).
            arg(_cmd);
}

bool AbstractData::prepareToSend() {
    if (isValid()) {
        return true;
    }

    return init();
}

void AbstractData::fromPakcage(const Package &pkg) {
    fromBytes(pkg.data);
}

AbstractData::~AbstractData() {

}


}
}
