/*
 * Copyright (C) 2021-2022 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "isignerdelegate.h"

namespace QH {

ISignerDelegate::ISignerDelegate() {

}

const QByteArray &ISignerDelegate::getSignature() const {
    return sign;
}

void ISignerDelegate::setSignature(const QByteArray &newSignature) {
    sign = newSignature;
}

const QByteArray &ISignerDelegate::getHash() const {
    return hash;

}

void ISignerDelegate::setHash(const QByteArray &newHash) {
    hash = newHash;

}

QDataStream &ISignerDelegate::fromStream(QDataStream &stream) {
    stream >> hash;
    stream >> sign;

    return stream;
}

QDataStream &ISignerDelegate::toStream(QDataStream &stream) const {
    stream << hash;
    stream << sign;

    return stream;
}
}
