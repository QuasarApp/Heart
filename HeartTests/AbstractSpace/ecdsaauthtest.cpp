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
    QByteArray pub, priv;

    QVERIFY(QH::AuthECDSA::makeKeys(pub, priv));

    QVERIFY(pub.length() && priv.length());

    ECDSA edsa(pub, priv);

    QVERIFY(!edsa.isValid());

    QVERIFY(!edsa.auth(60));

    QVERIFY(edsa.prepare());
    QVERIFY(edsa.isValid());

    QVERIFY(edsa.auth(60));
    QVERIFY(!edsa.auth(0));


}
