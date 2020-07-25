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
#include <quasarapp.h>

namespace NP {

#define THIS_CLASS QString::fromLatin1(typeid(*this).name())

ICrypto::ICrypto() {
    _keyPoolSizeMutex = new QMutex();
    _keysMutex = new QMutex();
    _taskMutex = new QMutex();
}

ICrypto::~ICrypto() {

    stop();
    waitForThreadFinished(9000000);

    if (!saveStorage()) {
        QuasarAppUtils::Params::log("save keys to storae is failed!",
                                    QuasarAppUtils::Error);
    }

    delete _keyPoolSizeMutex;
    delete  _keysMutex;
    delete  _taskMutex;
}

CryptoPairKeys ICrypto::getNextPair(const QString &accsessKey,
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

int ICrypto::getKeyPoolSize() const {
    return _keyPoolSize;
}

void ICrypto::setKeyPoolSize(int keyPoolSize) {
    _keyPoolSizeMutex->lock();
    _keyPoolSize = keyPoolSize;
    _keyPoolSizeMutex->unlock();

    start();
}

bool ICrypto::isValid() const {
    return isInited();
}

bool ICrypto::isInited() const {
    return _inited;
}

void ICrypto::setGenesisList(const QList<QByteArray>& list) {
    _keysMutex->lock();
    for (const auto& i : list) {
        _keys.insert(i, {});
    }
    _keysMutex->unlock();

    start();
}

bool ICrypto::toStorage(const QString &key) const {

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

bool ICrypto::fromStorage(const QByteArray &key) {
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

void ICrypto::generateKeysByTasks() {
    for (auto it = _generateTasks.begin(); it != _generateTasks.end(); ++it) {

        if (_stopGenerator) {
            return;
        }

        const auto&  value = _keys.value(it.key());
        if (value.isValid())
            continue;

        _keysMutex->lock();

        if (it.value() == RAND_KEY && _randomKeysPool.size()) {
            _keys[it.key()] = *_randomKeysPool.begin();
            _randomKeysPool.erase(_randomKeysPool.begin());
        } else {
            _keys[it.key()] = generate(it.value());
        }

        _keysMutex->unlock();
    }
}

void ICrypto::generateRandomKeys() {
    _keyPoolSizeMutex->lock();
    int size =  _keyPoolSize;
    _keyPoolSizeMutex->unlock();

    while (size > _randomKeysPool.size()) {

        if ((_stopGenerator))
            return;

        auto &&keys = generate();

        _keysMutex->lock();
        _randomKeysPool.insert(keys);
        _keysMutex->unlock();
    }
}

void ICrypto::run() {

    if (_stopGenerator) {
        return;
    }

    generateKeysByTasks();
    generateRandomKeys();
}

void ICrypto::stop() {
    _stopGenerator = true;
}

bool ICrypto::waitForGeneratekey(const QString& key, int timeout) const {
    return waitFor([this, &key](){return _keys.contains(key);}, timeout);
}

bool ICrypto::waitForThreadFinished(int timeout) const {
    return waitFor([this](){return !isRunning();}, timeout);
}

bool ICrypto::waitFor(const std::function<bool()> &checkFunc, int timeout) const {
    auto waitFor = timeout + QDateTime::currentMSecsSinceEpoch();
    while (!checkFunc() && waitFor > QDateTime::currentMSecsSinceEpoch()) {
        QCoreApplication::processEvents();
    }

    return checkFunc();
}

void ICrypto::loadAllKeysFromStorage() {
    auto list = QDir(storageLocation()).entryInfoList(QDir::Files | QDir::NoDotAndDotDot);

    for (const auto& file: list ) {
        fromStorage(file.absoluteFilePath().toLatin1());
    }
}

bool ICrypto::saveStorage() const {

    bool result = true;
    for (auto it = _keys.begin(); it != _keys.end(); ++it) {
        result = result && toStorage(it.key());
    }

    return result;
}

bool ICrypto::genKey(const QString &accessKey, const QByteArray &genesis) {

    if (accessKey.isEmpty())
        return false;

    QMutexLocker locker(_taskMutex);
    _generateTasks.insert(accessKey, genesis);
    return true;
}

QString ICrypto::storageLocation() const {
    return _storageLocation;
}

bool ICrypto::initStorageLocation(const QString &value) {
    QFile version(value + "/version");

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

        QFile version(value + "/version");

        if (!version.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            return false;
        }

        QDataStream stream(&version);
        stream << THIS_CLASS;
        version.close();

    } else {

        if (!version.open(QIODevice::ReadOnly)) {
            return false;
        }

        QDataStream stream(&version);
        QString versionName;
        stream >> versionName;
        version.close();

        if (THIS_CLASS != versionName) {
            return false;
        }

    }

    _storageLocation = value;

    return _storageLocation.size();

}

bool ICrypto::initDefaultStorageLocation(const QString &dirName) {
    auto storageLoation =
            QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) +
            "/crypto/" + dirName;

    if (dirName.isEmpty()) {
        storageLoation += THIS_CLASS;
    }

    if (!initStorageLocation(storageLoation)) {
        QuasarAppUtils::Params::log("CryptoStorage not inited",
                                    QuasarAppUtils::Error);

        return false;
    }

    loadAllKeysFromStorage();

    return _inited = true;
}

void ICrypto::clearStorage() const {
    QDir::root().remove(storageLocation());
}

}
