/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "package.h"
#include "streambase.h"

#include <QDataStream>
#include <QIODevice>

namespace QH {


StreamBase::StreamBase() = default;

StreamBase::~StreamBase() = default;

bool StreamBase::fromBytes(const QByteArray &data) {
    if (data.isEmpty())
        return false;

    QDataStream stream(data);
    fromStream(stream);
    return true;
}

QByteArray StreamBase::toBytes() const {
    QByteArray res;
    QDataStream stream(&res, QIODevice::WriteOnly);
    toStream(stream);
    return res;
}

StreamBase &StreamBase::operator=(const StreamBase &righ) {
    QByteArray res;
    QDataStream stream(&res, QIODevice::WriteOnly);
    righ.toStream(stream);
    toStream(stream);

    return *this;
}

QDataStream &operator<<(QDataStream &stream, const StreamBase &obj) {
    return (&obj)->toStream(stream);
}

QDataStream &operator>>(QDataStream &stream, StreamBase &obj) {
    return (&obj)->fromStream(stream);
}

}
