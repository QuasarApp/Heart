#include "rsakeyspool.h"
#include <QMutexLocker>

namespace ClientProtocol {

bool RSAKeysPool::take(QRSAEncryption::Rsa rsa, RSAKeyPair& res) {
    QMutexLocker locker(&multithread);

    if (pool.value(rsa).isEmpty()) {
        emit sigKeyTaked();
        return false;
    }

    res = pool.value(rsa).begin().value();
    return true;
}

void RSAKeysPool::addNewKey(QRSAEncryption::Rsa rsa, const RSAKeyPair& key) {
    QMutexLocker locker(&multithread);
    pool[rsa].insert(static_cast<int>(time(nullptr)), key);
}

int RSAKeysPool::size(QRSAEncryption::Rsa rsa) const {
    return pool.value(rsa).size();
}

}
