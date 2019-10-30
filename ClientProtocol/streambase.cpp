#include "package.h"
#include "streambase.h"

#include <QDataStream>

namespace ClientProtocol {


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

QVariantMap StreamBase::getMap() const {
    QVariantMap map;
    return toVariantMap(map);
}
}
