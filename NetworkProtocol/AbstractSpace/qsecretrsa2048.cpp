#include "cryptopairkeys.h"
#include "qsecretrsa2048.h"
#include <qrsaencryption.h>

namespace NP {

QSecretRSA2048::QSecretRSA2048() {
    qtSecret = new QRSAEncryption(QRSAEncryption::RSA_2048);
}

CryptoPairKeys QSecretRSA2048::generate(const QByteArray &genesis) const {
    QByteArray pubKey;
    QByteArray privKey;

    qtSecret->generatePairKey(pubKey, privKey, genesis);

    return {pubKey, privKey};
}

bool QSecretRSA2048::crypt(QByteArray *data,
                           const QByteArray &publicKey) {
    *data = qtSecret->encode(*data, publicKey);

    return data->size();
}

bool QSecretRSA2048::decrypt(QByteArray *cryptedData,
                             const QByteArray &privateKey) {
    *cryptedData = qtSecret->decode(*cryptedData, privateKey);

    return cryptedData->size();
}

bool QSecretRSA2048::sign(QByteArray *data,
                          const QByteArray &privateKey) {

    *data = qtSecret->signMessage(*data, privateKey);
    return data->size();

}

bool QSecretRSA2048::check(const QByteArray &signedData,
                           const QByteArray &publicKey) {

    return qtSecret->checkSignMessage(signedData, publicKey);
}


QSecretRSA2048::~QSecretRSA2048() {
    delete qtSecret;
}

}