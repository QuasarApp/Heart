/*
 * Copyright (C) 2018-2021 QuasarApp.
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
    if (_cmd)
        return _cmd;

    return generateCmd();
}

void AbstractData::setCmd(unsigned short cmd) {
    _cmd = cmd;
}

bool AbstractData::init() {
    if (typeid (*this).hash_code() == typeid(AbstractData).hash_code())
        return false;

    initCmd();

    return true;
}

unsigned short AbstractData::generateCmd() const {
    return H_16(*this);
}

void AbstractData::initCmd() {
    setCmd(generateCmd());
}

AbstractData::AbstractData() {
    setCmd(0);
}

bool AbstractData::toPackage(Package &package,
                             unsigned int triggerHash) const {

    if (!isValid()) {
        return false;
    }

    package.data = toBytes();

    package.hdr.command = cmd();
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
    stream << cmd();
    return stream;
}

bool AbstractData::isValid() const {
    return cmd();
}

bool AbstractData::copyFrom(const AbstractData *other) {

    return other;
}

QString AbstractData::toString() const {
    return QString("Object: type:%0, command:%1").
            arg(typeid(*this).name()).
            arg(cmd());
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
