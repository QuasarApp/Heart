/*
 * Copyright (C) 2018-2022 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "package.h"
#include "streambase.h"

#include <QDataStream>
#include <QIODevice>
#include <QSharedPointer>

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

unsigned int StreamBase::typeId() const {
    return typeid (*this).hash_code();
}

QDataStream &operator<<(QDataStream &stream, const StreamBase &obj) {
    return (&obj)->toStream(stream);
}

QDataStream &operator>>(QDataStream &stream, StreamBase &obj) {
    return (&obj)->fromStream(stream);
}

}
