#include "cryptopairkeys.h"
#include "qsecretrsa2048.h"
#include <qrsaencryption.h>

namespace NP {

QSecretRSA2048::QSecretRSA2048(const QList<QByteArray> &genesises) {
    for (const auto &genesis: genesises) {
        _genesisKeys[genesis] = {};
    }
}

CryptoPairKeys QSecretRSA2048::getFromGenesis(const QByteArray &genesis) {
    return _genesisKeys.value(genesis);
}

CryptoPairKeys QSecretRSA2048::generate() const {
    QByteArray pubKey;
    QByteArray privKey;

    QRSAEncryption::generatePairKey(pubKey, privKey, QRSAEncryption::RSA_2048);

    return {pubKey, privKey};
}

void QSecretRSA2048::run() {
    ICrypto::run();

    for ( auto genesis = _genesisKeys.begin(); genesis != _genesisKeys.end(); ++genesis) {
        if (genesis.value().isValid())
            continue;

        QByteArray pubKey;
        QByteArray privKey;

        QRSAEncryption::generatePairKey(pubKey, privKey, QRSAEncryption::RSA_2048);

        _genesisKeys[genesis.key()] = {pubKey, privKey};
    }
}


}
