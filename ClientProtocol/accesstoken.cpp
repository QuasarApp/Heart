#include "accesstoken.h"
#include <QCryptographicHash>
#include <QDataStream>

namespace ClientProtocol {

QByteArray AccessToken::generate(const QByteArray &entropy) {

    QByteArray result = QCryptographicHash::hash(entropy, QCryptographicHash::Sha256);

    srand(static_cast<unsigned int>(time(nullptr)));
    for (int i = 0; i < 256; ++i) {
        char byte = static_cast<char>(rand() % 256);
        result.push_back(byte);
    }

    return QCryptographicHash::hash(result, QCryptographicHash::Sha256);
}

AccessToken::AccessToken(int duration, const QByteArray &entropy) {
    _duration = static_cast<int>(QDateTime::currentSecsSinceEpoch()) + duration;
    _data = generate(entropy);
}

AccessToken::AccessToken() = default;

AccessToken::AccessToken(const AccessToken &other) = default;

bool AccessToken::isValid() const {
    return _duration > QDateTime::currentSecsSinceEpoch() &&
            _data.size() == 32;
}

bool AccessToken::operator ==(const AccessToken &other) const {
    return _data == other._data;
}

AccessToken& AccessToken::operator =(const AccessToken &other) = default;

QDataStream &AccessToken::fromStream(QDataStream &stream) {
    stream >> _data;
    stream >> _duration;
    return stream;
}

QDataStream &AccessToken::toStream(QDataStream &stream) const {
    stream << _data;
    stream << _duration;
    return stream;
}

QDataStream &operator>>(QDataStream &stream, AccessToken &token) {
    return token.fromStream(stream);
}

QDataStream &operator<<(QDataStream &stream, const AccessToken &token) {
    return token.toStream(stream);
}

uint qHash(const AccessToken &token) {
    return qHash(token._data);
}
}

