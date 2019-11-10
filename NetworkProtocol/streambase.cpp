#include "package.h"
#include "streambase.h"

#include <QDataStream>

namespace NetworkProtocol {


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

QDataStream &operator<<(QDataStream &stream, const StreamBase &obj) {
    return (&obj)->toStream(stream);
}

QDataStream &operator>>(QDataStream &stream, StreamBase &obj) {
    return (&obj)->fromStream(stream);
}

}
