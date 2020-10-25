/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "accesstoken.h"
#include <QCryptographicHash>
#include <QDataStream>

namespace QH {

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

AccessToken::AccessToken(const QByteArray &other) {
    fromBytes(other);
}

AccessToken::AccessToken() = default;

AccessToken::AccessToken(const AccessToken &other) = default;

bool AccessToken::isValid() const {
    return (_duration > QDateTime::currentSecsSinceEpoch()) &&
            (_data.size() == 32);
}

void AccessToken::clear()
{
    _data.clear();
    _duration = 0;
}

bool AccessToken::operator ==(const AccessToken &other) const {
    return _data == other._data;
}

bool AccessToken::operator !=(const AccessToken &other) const {
    return !operator==(other);
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

uint qHash(const AccessToken &token) {
    return qHash(token._data);
}
}

