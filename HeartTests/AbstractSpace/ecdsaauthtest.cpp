/*
 * Copyright (C) 2022-2022 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "ecdsaauthtest.h"
#include "authecdsa.h"
#include <QtTest>

/*
 * test class
 */
class ECDSA: public QH::AuthECDSA {

public:
    ECDSA(const QByteArray &publicKey, const QByteArray &privKey) {
        setPublicKey(publicKey);
        _priv = privKey;
    }

    // AsyncKeysAuth interface
protected:
    QByteArray getPrivateKey() const override {
        return _priv;
    };

private:
    QByteArray _priv;

};

ECDSAAuthTest::ECDSAAuthTest() {

}

ECDSAAuthTest::~ECDSAAuthTest() {

}

void ECDSAAuthTest::test() {
    // create a publick and private keys array.
    QByteArray pub, priv;

    // make public and private keys.
    QVERIFY(QH::AuthECDSA::makeKeys(pub, priv));

    // check createed keys. should be larget then 0.
    QVERIFY(pub.length() && priv.length());

    // create test auth object using ecdsa algorithm
    ECDSA edsa(pub, priv);

    // The terst object should be invalid because it is not prepared.
    QVERIFY(!edsa.isValid());

    // the authetication should be failed bacause ecdsa class is invalid.
    QVERIFY(!edsa.auth(600));

    // prepare an authentication object.
    QVERIFY(edsa.prepare());
    // the prepared object should be valid.
    QVERIFY(edsa.isValid());

    // authentication should be finished successful because auth object contains prepared valid signature.
    QVERIFY(edsa.auth(600));
    // authentication should be failed because the time range is depricated.
    QVERIFY(!edsa.auth(0));




}
