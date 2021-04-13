/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#include "cryptopairkeys.h"
#include "icrypto.h"
#include "keystorage.h"
#include <quasarapp.h>

#include <QCoreApplication>
#include <QDataStream>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QMutex>
#include <QStandardPaths>
#include "config.h"

namespace QH {

#define THE_CLASS(x) QString::fromLatin1(typeid(*x).name())
#define VERSION_FILE "version"

KeyStorage::KeyStorage(ICrypto * cryptoMethod) {
    _keyPoolSizeMutex = new QMutex();
    _keysMutex = new QMutex();
    _taskMutex = new QMutex();

    _cryptoMethod = cryptoMethod;

    assert(_cryptoMethod);
}

KeyStorage::~KeyStorage() {

    stop();
    waitForThreadFinished(WAIT_TIME);

    if (!saveStorage()) {
        QuasarAppUtils::Params::log("save keys to storae is failed!",
                                    QuasarAppUtils::Error);
    }

    delete _keyPoolSizeMutex;
    delete  _keysMutex;
    delete  _taskMutex;
    delete  _cryptoMethod;
}

CryptoPairKeys KeyStorage::getNextPair(const QString &accsessKey,
                                    const QByteArray& genesis,
                                    int timeout) {

    if (!isInited() && genesis == RAND_KEY) {
        QuasarAppUtils::Params::log("You want get the random crypto keys pair in a not initialized crypto object.",
                                    QuasarAppUtils::Error);
        return CryptoPairKeys{};
    }

    if (_keyPoolSize <= 0) {
        return CryptoPairKeys{};
    }

    if (!genKey(accsessKey, genesis)) {
        return {};
    }

    start();

    if (!waitForGeneratekey(accsessKey, timeout)) {
        return CryptoPairKeys{};
    }

    QMutexLocker locker(_keysMutex);

    return _keys.value(accsessKey, {});
}

int KeyStorage::getKeyPoolSize() const {
    return _keyPoolSize;
}

void KeyStorage::setKeyPoolSize(int keyPoolSize) {
    _keyPoolSizeMutex->lock();
    _keyPoolSize = keyPoolSize;
    _keyPoolSizeMutex->unlock();

    start();
}

bool KeyStorage::isValid() const {
    return isInited();
}

bool KeyStorage::isInited() const {
    return _inited;
}

bool KeyStorage::crypt(QByteArray *data, const QByteArray &publicKey) {
    return _cryptoMethod->crypt(data, publicKey);
}

bool KeyStorage::decrypt(QByteArray *cryptedData, const QByteArray &privateKey) {
    return _cryptoMethod->decrypt(cryptedData, privateKey);
}

bool KeyStorage::sign(QByteArray *data, const QByteArray &privateKey) {
    return _cryptoMethod->sign(data, privateKey);
}

QByteArray KeyStorage::extractSign(const QByteArray &data) {
    return _cryptoMethod->extractSign(data);
}

QByteArray KeyStorage::concatSign(const QByteArray &data, const QByteArray &sign) {
    return _cryptoMethod->concatSign(data, sign);
}

bool KeyStorage::check(const QByteArray &signedData, const QByteArray &publicKey) {
    return _cryptoMethod->check(signedData, publicKey);
}

void KeyStorage::setGenesisList(const QList<QByteArray>& list) {
    _keysMutex->lock();
    for (const auto& i : list) {
        _keys.insert(i, {});
    }
    _keysMutex->unlock();

    start();
}

bool KeyStorage::toStorage(const QString &key) const {

    if (!isValid())
        return false;

    _keysMutex->lock();
    CryptoPairKeys value = _keys.value(key);
    _keysMutex->unlock();

    auto filePath = storageLocation() + "/" + key;

    QFile file(filePath);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        return false;
    }

    file.setPermissions(QFile::Permission::ReadOwner | QFile::Permission::WriteOwner);

    QDataStream stream(&file);

    stream << value;

    file.close();

    return true;
}

bool KeyStorage::fromStorage(const QByteArray &key) {
    auto filePath = storageLocation() + "/" + key;

    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    QDataStream stream(&file);

    CryptoPairKeys value;
    stream >> value;

    file.close();

    _keys.insert(key, value);

    return value.isValid();
}

void KeyStorage::generateKeysByTasks() {
    _taskMutex->lock();
    auto tasks =  _generateTasks;
    _taskMutex->unlock();

    for (auto it = tasks.begin(); it != tasks.end(); ++it) {

        if (_stopGenerator) {
            return;
        }

        const auto&  value = _keys.value(it.key());
        if (!value.isValid()) {

            _keysMutex->lock();

            if (it.value() == RAND_KEY && _randomKeysPool.size()) {
                _keys[it.key()] = *_randomKeysPool.begin();
                _randomKeysPool.erase(_randomKeysPool.begin());
            } else {
                _keys[it.key()] = _cryptoMethod->generate(it.value());
            }

            _keysMutex->unlock();

        }

        _taskMutex->lock();
        _generateTasks.remove(it.key());
        _taskMutex->unlock();

    }
}

void KeyStorage::generateRandomKeys() {
    _keyPoolSizeMutex->lock();
    int size =  _keyPoolSize;
    _keyPoolSizeMutex->unlock();

    while (size > _randomKeysPool.size()) {

        if ((_stopGenerator))
            return;

        auto &&keys = _cryptoMethod->generate();

        _keysMutex->lock();
        _randomKeysPool.insert(keys);
        _keysMutex->unlock();
    }
}

void KeyStorage::run() {

    if (_stopGenerator) {
        return;
    }

    _keyPoolSizeMutex->lock();
    int keyPoolSize = _keyPoolSize;
    _keyPoolSizeMutex->unlock();

    while ((_generateTasks.size() || keyPoolSize > _randomKeysPool.size())
           && !_stopGenerator) {

        generateKeysByTasks();
        generateRandomKeys();

        _keyPoolSizeMutex->lock();
        keyPoolSize = _keyPoolSize;
        _keyPoolSizeMutex->unlock();
    }
}

void KeyStorage::stop() {
    _stopGenerator = true;
}

bool KeyStorage::waitForGeneratekey(const QString& key, int timeout) const {
    return waitFor([this, &key](){return _keys.contains(key);}, timeout);
}

bool KeyStorage::waitForThreadFinished(int timeout) const {
    return waitFor([this](){return !isRunning();}, timeout);
}

bool KeyStorage::waitFor(const std::function<bool()> &checkFunc, int timeout) const {
    auto waitFor = timeout + QDateTime::currentMSecsSinceEpoch();
    while (!checkFunc() && waitFor > QDateTime::currentMSecsSinceEpoch()) {
        QCoreApplication::processEvents();
    }

    return checkFunc();
}

void KeyStorage::loadAllKeysFromStorage() {
    auto list = QDir(storageLocation()).entryInfoList(QDir::Files | QDir::NoDotAndDotDot);

    for (const auto& file: list ) {
        if (file.fileName() != VERSION_FILE) {
            fromStorage(file.fileName().toLatin1());
        }
    }
}

bool KeyStorage::saveStorage() const {

    bool result = true;
    for (auto it = _keys.begin(); it != _keys.end(); ++it) {
        result = result && toStorage(it.key());
    }

    return result;
}

bool KeyStorage::genKey(const QString &accessKey, const QByteArray &genesis) {

    if (accessKey.isEmpty())
        return false;

    QMutexLocker locker(_taskMutex);
    _generateTasks.insert(accessKey, genesis);
    return true;
}

QString KeyStorage::storageLocation() const {
    return _storageLocation;
}

bool KeyStorage::initStorageLocation(const QString &value) {
    QFile version(value + "/" + VERSION_FILE);

    if (!QFile::exists(value)) {

        if (!QDir().mkpath(value)) {
            QuasarAppUtils::Params::log(" fail to create a key storagge. Into "
                                        + value, QuasarAppUtils::Error);
            return false;
        }

        QFile::setPermissions(value,
                              QFile::Permission::ExeOwner |
                              QFile::Permission::ReadOwner |
                              QFile::Permission::WriteOwner);

        QFile version(value + "/" + VERSION_FILE);

        if (!version.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            return false;
        }

        QDataStream stream(&version);
        stream << THE_CLASS(_cryptoMethod);
        version.close();

    } else {

        if (!version.open(QIODevice::ReadOnly)) {
            return false;
        }

        QDataStream stream(&version);
        QString versionName;
        stream >> versionName;
        version.close();

        if (THE_CLASS(_cryptoMethod) != versionName) {
            return false;
        }

    }

    _storageLocation = value;

    return _storageLocation.size();

}

bool KeyStorage::initDefaultStorageLocation(const QString &dirName) {
    auto storageLoation =
            QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) +
            "/" + dirName;

    if (dirName.isEmpty()) {
        storageLoation += THE_CLASS(_cryptoMethod);
    }

    storageLoation += "/crypto/";

    if (!initStorageLocation(storageLoation)) {
        QuasarAppUtils::Params::log("CryptoStorage not inited",
                                    QuasarAppUtils::Error);

        return false;
    }

    loadAllKeysFromStorage();

    return _inited = true;
}

void KeyStorage::clearStorage() const {
    QDir::root().remove(storageLocation());
}

}
