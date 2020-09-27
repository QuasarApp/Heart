/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#include "cryptopairkeys.h"

#include <QDataStream>
namespace QH {
CryptoPairKeys::CryptoPairKeys() {

}

CryptoPairKeys::CryptoPairKeys(const QByteArray &pubKey, const QByteArray &privKey) {
    setPrivKey(privKey);
    setPublicKey(pubKey);
    setBits(privKey.size() * 8);
}

bool CryptoPairKeys::isValid() const {
    return _privKey.size() && _publicKey.size() && !(_bits % 8) && _bits > 1;
}

QByteArray CryptoPairKeys::privKey() const {
    return _privKey;
}

void CryptoPairKeys::setPrivKey(const QByteArray &privKey) {
    _privKey = privKey;
}

QByteArray CryptoPairKeys::publicKey() const {
    return _publicKey;
}

void CryptoPairKeys::setPublicKey(const QByteArray &publicKey) {
    _publicKey = publicKey;
}

int CryptoPairKeys::bits() const {
    return _bits;
}

void CryptoPairKeys::setBits(int bits) {
    _bits = bits;
}

QDataStream &CryptoPairKeys::fromStream(QDataStream &stream) {

    stream >> _publicKey;
    stream >> _privKey;
    stream >> _bits;

    return stream;
}

QDataStream &CryptoPairKeys::toStream(QDataStream &stream) const {
    stream << _publicKey;
    stream << _privKey;
    stream << _bits;

    return stream;

}

bool operator ==(const CryptoPairKeys &left, const CryptoPairKeys &right) {
    return !(left != right);
}

bool operator !=(const CryptoPairKeys &left, const CryptoPairKeys &right) {
    return left._privKey != right._privKey || left._publicKey != right._publicKey;
}

uint qHash(const CryptoPairKeys &value) {
    return qHash(value.privKey());
}

}
