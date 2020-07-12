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

ICrypto::ICrypto() {
    _keyPoolSizeMutex = new QMutex();
    _keysMutex = new QMutex();
    _taskMutex = new QMutex();
    _storageLocation = QStandardPaths::locate(QStandardPaths::DataLocation, "") + "/KeysStorage";

    loadAllKeysFromStorage();
}

ICrypto::~ICrypto() {
    delete _keyPoolSizeMutex;
    delete  _keysMutex;
}

CryptoPairKeys ICrypto::getNextPair(const QByteArray &accsessKey,
                                    const QByteArray& genesis,
                                    int timeout) {
    if (_keyPoolSize <= 0) {
        return CryptoPairKeys{};
    }

    if (genesis.isEmpty()) {
        if (!genRandomKey(accsessKey)) {
            return {};
        }
    } else {
        if (!genKey(genesis, accsessKey)) {
            return {};
        }
    }

    start();

    if (!waitForGeneratekey(genesis, timeout)) {
        return CryptoPairKeys{};
    }

    QMutexLocker locker(_keysMutex);

    auto &list = _keys[genesis];

    if (!list.size()) {
        return {};
    }
    return list.takeFirst();
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
    QList<CryptoPairKeys> value = _keys.value(genesis);
    auto filePath = storageLocation() + "/" +
            QCryptographicHash::hash(genesis, QCryptographicHash::Md5).toBase64();

    QFile key(filePath);

    if (!key.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        return false;
    }

    key.setPermissions(QFile::Permission::ReadOwner | QFile::Permission::WriteOwner);

    QDataStream stream(&key);

    stream << value;

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

    QList<CryptoPairKeys> value;
    stream >> value;

    key.close();

    _keys.insert(genesis, value);

    return value.size();
}

void ICrypto::run() { 

    for (auto it = _generateTasks.begin(); it != _generateTasks.end(); ++it) {
        const auto&  values = _keys.value(it.key());
        if (values.size() && values.first().isValid())
            continue;

        auto &&keys = generate((it.value() != RAND_KEY)? it.value(): "");


        _keysMutex->lock();
        _keys[it.key()].push_back(keys);
        _keysMutex->unlock();
    }

    _keyPoolSizeMutex->lock();
    int size =  _keyPoolSize ;
    _keyPoolSizeMutex->unlock();

    while (size > _keys[RAND_KEY].size()) {
        auto &&keys = generate();

        _keysMutex->lock();
        _keys[RAND_KEY].push_back(keys);
        _keysMutex->unlock();
    }
}

bool ICrypto::waitForGeneratekey(const QByteArray& genesis, int timeout) const {

    auto waitFor = timeout + QDateTime::currentMSecsSinceEpoch();
    while (!_keys[genesis].size() && waitFor > QDateTime::currentMSecsSinceEpoch()) {
        QCoreApplication::processEvents();
    }

    return _keys[genesis].size();
}

void ICrypto::loadAllKeysFromStorage() {
    auto list = QDir(storageLocation()).entryInfoList();

    for (const auto& file: list ) {
        fromStorage(file.absoluteFilePath().toLatin1());
    }
}

bool ICrypto::genKey(const QByteArray &genesis, QByteArray accessKey) {
    if (genesis.isEmpty())
        return false;

    if (accessKey.isEmpty())
        accessKey = genesis;

    QMutexLocker locker(_taskMutex);
    _generateTasks.insert(accessKey, genesis);
    return true;
}

bool ICrypto::genRandomKey(const QByteArray &accessKey) {
    QMutexLocker locker(_taskMutex);
    _generateTasks.insert(accessKey, RAND_KEY);
    return true;
}

QString ICrypto::storageLocation() const {
    return _storageLocation;
}

void ICrypto::setStorageLocation(const QString &value) {
    if (!QFile::exists(value)) {
        if (!QDir().mkpath(_storageLocation)) {
            QuasarAppUtils::Params::log(" fail to create a key storagge. Into "
                                        + _storageLocation,
                                        QuasarAppUtils::Error);
            return;
        }
        QFile::setPermissions(_storageLocation,
                              QFile::Permission::ExeOwner |
                              QFile::Permission::ReadOwner |
                              QFile::Permission::WriteOwner);

    }
    _storageLocation = value;

}

}
