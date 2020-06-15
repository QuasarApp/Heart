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

CryptoPairKeys ICrypto::getNextPair(const QByteArray& genesis, int timeout) {
    if (_keyPoolSize <= 0) {
        return CryptoPairKeys{};
    }

    start();

    if (!waitForGeneratekey(genesis, timeout)) {
        return CryptoPairKeys{};
    }

    _keysMutex->lock();
    auto key = _keys.values(genesis).takeFirst();
    _keysMutex->unlock();

    return key;
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

void ICrypto::setGenesisList(const QList<QByteArray>& list) {
    _keysMutex->lock();
    for (const auto& i : list) {
        _keys.insert(i, {});
    }
    _keysMutex->unlock();

    start();
}

void ICrypto::run() {
    _keyPoolSizeMutex->lock();
    int size =  _keyPoolSize ;
    _keyPoolSizeMutex->unlock();

    for (auto it = _keys.begin(); it != _keys.end(); ++it) {
        if (it.key() != RAND_KEY) {
            if (it.value().isValid())
                continue;

            auto &&keys = generate(it.key());

            _keysMutex->lock();
            _keys.insertMulti(it.key(), keys);
            _keysMutex->unlock();

        } else {

            while (size > _keys.size()) {
                auto &&keys = generate();

                _keysMutex->lock();
                _keys.insertMulti(RAND_KEY, keys);
                _keysMutex->unlock();
            }
        }
    }
}

bool ICrypto::waitForGeneratekey(const QByteArray& genesis, int timeout) const {

    auto waitFor = timeout + QDateTime::currentMSecsSinceEpoch();
    while (!_keys.values(genesis).size() && waitFor > QDateTime::currentMSecsSinceEpoch()) {
        QCoreApplication::processEvents();
    }

    return _keys.values(genesis).size();
}

}
