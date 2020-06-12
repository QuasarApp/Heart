#include "cryptopairkeys.h"
#include "icrypto.h"
#include <QCoreApplication>

#include <QDateTime>
#include <QMutex>
#include <QMutexLocker>

namespace NP {

ICrypto::ICrypto() {
    _keyPoolSizeMutex = new QMutex();
    _keysMutex = new QMutex();
}

ICrypto::~ICrypto() {
    delete _keyPoolSizeMutex;
    delete  _keysMutex;
}

CryptoPairKeys &&ICrypto::getNextPair(int timeout) {
    if (_keyPoolSize <= 0) {
        return std::move(CryptoPairKeys{});
    }

    start();

    if (!waitForGeneratekey(timeout)) {
        return std::move(CryptoPairKeys{});
    }

    _keysMutex->lock();
    auto key = _keys.takeFirst();
    _keysMutex->unlock();

    return std::move(key);
}

int ICrypto::getKeyPoolSize() const {
    return _keyPoolSize;
}

void ICrypto::setKeyPoolSize(int keyPoolSize) {
    _keyPoolSizeMutex->lock();
    _keyPoolSize = keyPoolSize;
    _keyPoolSizeMutex->unlock();

    start();
}

void ICrypto::run() {
    _keyPoolSizeMutex->lock();
    int size =  _keyPoolSize ;
    _keyPoolSizeMutex->unlock();

    while (size > _keys.size()) {
        auto &&keys = generate();

        _keysMutex->lock();
        _keys.push_back(keys);
        _keysMutex->unlock();
    }
}

bool ICrypto::waitForGeneratekey(int timeout) const {

    auto waitFor = timeout + QDateTime::currentMSecsSinceEpoch();
    while (!_keys.size() && waitFor > QDateTime::currentMSecsSinceEpoch()) {
        QCoreApplication::processEvents();
    }

    return _keys.size();
}

}
