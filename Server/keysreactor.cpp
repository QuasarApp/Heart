#include "keysreactor.h"

ClientProtocol::RSAKeysPool *KeysReactor::getPool() {
    return &_pool;
}

int KeysReactor::getPoolSize() const {
    return _poolSize;
}

void KeysReactor::setPoolSize(int value) {
    if (_poolSize != value) {
        _poolSize = value;
        handleGenerateNewKeys();
    }
}

void KeysReactor::generateKeys(QRSAEncryption::Rsa rsa) {
    QByteArray pub, priv;

    if (_poolSize > _pool.size(rsa)) {
        QRSAEncryption::generatePairKey(pub, priv, rsa);
        _pool.addNewKey(rsa, {pub, priv});
    }

    return;
}

void KeysReactor::handleGenerateNewKeys() {

    auto generatorFunc = [this] (QRSAEncryption::Rsa rsa) {

        if (_mutexs[rsa]) {
            return;
        }

        _mutexs[rsa] = true;

        QByteArray pub, priv;

        while (_poolSize > _pool.size(rsa)) {
            QRSAEncryption::generatePairKey(pub, priv, rsa);
            _pool.addNewKey(rsa, {pub, priv});
        }

        _mutexs[rsa] = false;

        return;
    };

    _futures.insert(QRSAEncryption::RSA_64,
                    QtConcurrent::run(generatorFunc, QRSAEncryption::RSA_64));
    _futures.insert(QRSAEncryption::RSA_128,
                    QtConcurrent::run(generatorFunc, QRSAEncryption::RSA_128));
    _futures.insert(QRSAEncryption::RSA_256,
                    QtConcurrent::run(generatorFunc, QRSAEncryption::RSA_256));
    _futures.insert(QRSAEncryption::RSA_512,
                    QtConcurrent::run(generatorFunc, QRSAEncryption::RSA_512));


}

KeysReactor::KeysReactor(bool ForceGenerateKey, QObject *ptr):
    QObject (ptr) {

    _mutexs[QRSAEncryption::RSA_64] = false;
    _mutexs[QRSAEncryption::RSA_128] = false;
    _mutexs[QRSAEncryption::RSA_256] = false;
    _mutexs[QRSAEncryption::RSA_512] = false;

    if (ForceGenerateKey) {
        generateKeys(QRSAEncryption::RSA_64);
        generateKeys(QRSAEncryption::RSA_128);
        generateKeys(QRSAEncryption::RSA_256);
        generateKeys(QRSAEncryption::RSA_512);
    }

    handleGenerateNewKeys();

    connect(&_pool, &ClientProtocol::RSAKeysPool::sigKeyTaked,
            this, &KeysReactor::handleGenerateNewKeys);
}

KeysReactor::~KeysReactor() {
    for (auto && i: _futures) {
        i.cancel();
        i.waitForFinished();
    }
}
