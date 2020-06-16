#include "cryptopairkeys.h"
#include "icrypto.h"
#include <QCoreApplication>

#include <QDateTime>
#include <QFile>
#include <QMutex>
#include <QMutexLocker>
#include <QCryptographicHash>
#include <QDataStream>
#include <QStandardPaths>
#include <QDir>

namespace NP {

ICrypto::ICrypto() {
    _keyPoolSizeMutex = new QMutex();
    _keysMutex = new QMutex();
    _storageLocation = QStandardPaths::locate(QStandardPaths::DataLocation, "") + "/KeysStorage";

    loadAllKeysFromStorage();
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

bool ICrypto::toStorage(const QByteArray &genesis) {
    auto pair = _keys.value(genesis);
    auto filePath = storageLocation() + "/" +
            QCryptographicHash::hash(genesis, QCryptographicHash::Md5).toBase64();

    QFile key(filePath);

    if (!key.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        return false;
    }

    QDataStream stream(&key);

    stream << pair.publicKey();
    stream << pair.privKey();

    key.close();

    return true;
}

bool ICrypto::fromStorage(const QByteArray &genesis) {
    auto filePath = genesis;

    QFile key(filePath);

    if (!key.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        return false;
    }

    QDataStream stream(&key);

    QByteArray pubKey, privKey;

    stream >> pubKey;
    stream >> privKey;

    key.close();

    _keys[genesis] = CryptoPairKeys{pubKey, privKey};

    return _keys[genesis].isValid();
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

void ICrypto::loadAllKeysFromStorage() {
    auto list = QDir(storageLocation()).entryInfoList();

    for (const auto& file: list ) {
        fromStorage(file.absoluteFilePath().toLatin1());
    }
}

QString ICrypto::storageLocation() const {
    return _storageLocation;
}

void ICrypto::setStorageLocation(const QString &value) {
    _storageLocation = value;
}

}
