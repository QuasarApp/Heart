/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#include "abstractnodetest.h"
#include "testutils.h"

#include <abstractnode.h>
#include <keystorage.h>
#include <ping.h>
#include <qsecretrsa2048.h>

class TestingClient: public QH::AbstractNode {


    // AbstractNode interface
public:
    const QH::PKG::Ping& getPing() const {
        return _ping;
    }

protected:
    void incomingData(QH::PKG::AbstractData *pkg, const QH::HostAddress &sender) {
        Q_UNUSED(sender);

        auto ping = dynamic_cast<QH::PKG::Ping*>(pkg);
        if (ping)
            _ping.setAnsver(ping->ansver());
    }

private:
    QH::PKG::Ping _ping;
};

AbstractNodeTest::AbstractNodeTest() {
    _nodeA = new QH::AbstractNode();
    _nodeB = new TestingClient();
}

AbstractNodeTest::~AbstractNodeTest() {
    delete _nodeA;
    delete _nodeB;
}

void AbstractNodeTest::test() {
    QVERIFY(connectTest());
    QVERIFY(sendDataTest());
}

bool AbstractNodeTest::connectTest() {

    if (!_nodeA->run(TEST_LOCAL_HOST, TEST_PORT)) {
        return false;
    }

    return connectFunc(_nodeB, TEST_LOCAL_HOST, TEST_PORT);
}

bool AbstractNodeTest::sendDataTest() {

    auto request = [this](){
        return _nodeB->ping(QH::HostAddress(TEST_LOCAL_HOST, TEST_PORT));
    };

    auto client = dynamic_cast<TestingClient*>(_nodeB);

    auto check = [client](){
        return client->getPing().ansver();
    };

    return funcPrivateConnect(request, check);
}

template<class Crypto>
bool validationCrypto() {
    // create crypto oject
    auto crypto = new QH::KeyStorage(new Crypto());


    // get test pair keys
    auto keys = crypto->getNextPair("TEST_KEY");

    // must be failed becouse crypto object still not inited.
    if (keys.isValid()) {
        delete crypto;
        return false;
    }

    if (!crypto->initDefaultStorageLocation()) {
        delete crypto;
        return false;
    }

    // get test pair keys
    keys = crypto->getNextPair("TEST_KEY");

    // chekck keys
    if (!keys.isValid()) {
        delete crypto;
        return false;
    }

    // remove crypto object, after remove crypto object most be save all generated keys
    delete crypto;

    // second initialisin of crypto object
    crypto = new QH::KeyStorage(new Crypto());
    if (!crypto->initDefaultStorageLocation()) {
        delete crypto;
        return false;
    }

    // check get generated key pair
    if (keys != crypto->getNextPair("TEST_KEY", 0)) {
        delete crypto;
        return false;
    }

    QByteArray msg = "test_message";

    // check sign data
    if (!crypto->sign(&msg, keys.privKey())) {
        delete crypto;
        return false;
    }

    if (!crypto->check(msg, keys.publicKey())) {
        delete crypto;
        return false;
    }

    // check genesis generation of keys
    auto ThisIsKey = crypto->getNextPair("key", "this is key");
    auto ThisIsKey2 = crypto->getNextPair("key2", "this is key");

    if (ThisIsKey != ThisIsKey2) {
        delete crypto;
        return false;
    }


    delete crypto;

    crypto = new QH::KeyStorage(new Crypto());
    if (!crypto->initDefaultStorageLocation()) {
        delete crypto;
        return false;
    }

    auto lastKeys = crypto->getNextPair("key2", RAND_KEY, 0);
    return lastKeys == ThisIsKey2;
}

bool AbstractNodeTest::testICtypto() {
    // check
    if (!validationCrypto<QH::QSecretRSA2048>()) {
        return false;
    }

    return true;
}
