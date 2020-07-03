#include "dbid.h"
namespace NP {

DbId::DbId() = default;

DbId::DbId(const QByteArray &raw) {
    fromRaw(raw);
}

DbId::DbId(const QString &base64) {
    fromBase64(base64.toLatin1());
}

bool DbId::fromBase64(const QByteArray &base64) {
    return fromRaw(QByteArray::fromBase64(base64));;
}

bool DbId::fromRaw(const QByteArray &raw) {
    _data = raw;
    return isValid();
}

QByteArray DbId::toBase64() const {
    return _data.toBase64();
}

QByteArray DbId::toRaw() const {
    return _data;
}

bool DbId::isValid() const {
    return _data.size() == 33;
}

void DbId::clear() {
    _data.clear();
}

unsigned char DbId::prefix() const {
    if (_data.size())
        return _data[0];

    return 0;
}

QDataStream &DbId::fromStream(QDataStream &stream) {
    stream >> _data;
    return stream;
}

QDataStream &DbId::toStream(QDataStream &stream) const {
    stream << _data;
    return stream;
}

bool operator==(const DbId &left, const DbId &other) {
    return left._data == other._data;
}
}
