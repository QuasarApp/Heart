/*
 * Copyright (C) 2021-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "asynckeysauth.h"
#include "QCryptographicHash"
#include <time.h>

namespace QH {

AsyncKeysAuth::AsyncKeysAuth() {

}

bool AsyncKeysAuth::auth(int allowedTimeRangeSec) const {

    int diff = time(0) - _unixTime;

    if (diff < 0) {
        return false;
    }

    if (diff >= allowedTimeRangeSec) {
        return false;
    }

    QByteArray data = _publicKey;
    data.insert(0, reinterpret_cast<const char*>(&_unixTime),
                sizeof(_unixTime));

    return checkSign(data, _signature, _publicKey);
}

bool AsyncKeysAuth::prepare() {
    _unixTime = time(0);

    QByteArray data = _publicKey;
    data.insert(0, reinterpret_cast<const char*>(&_unixTime),
                sizeof(_unixTime));

    setSignature(signMessage(data, getPrivateKey()));

    return isValid();
}

unsigned int AsyncKeysAuth::unixTime() const {
    return _unixTime;
}

void AsyncKeysAuth::setUnixTime(unsigned int newUnixTime) {
    _unixTime = newUnixTime;
}

const QByteArray &AsyncKeysAuth::signature() const {
    return _signature;
}

const QByteArray &AsyncKeysAuth::publicKey() const {
    return _publicKey;
}

void AsyncKeysAuth::setPublicKey(const QByteArray &newPublicKey) {
    _publicKey = newPublicKey;
}

bool AsyncKeysAuth::isValid() const {
    return _publicKey.size() && _signature.size() && _unixTime;
}

void AsyncKeysAuth::setSignature(const QByteArray &newSignature) {
    _signature = newSignature;
}

}
