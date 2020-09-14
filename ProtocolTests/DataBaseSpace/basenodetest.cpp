#include "basenodetest.h"
#include "basenodeunittests.h"
#include "testutils.h"

#include <databasenode.h>
#include <hostaddress.h>
#include <keystorage.h>
#include <ping.h>
#include <qsecretrsa2048.h>

class TestingBaseClient: public NP::DataBaseNode {


    // AbstractNode interface
public:
    const NP::Ping& getPing() const {
        return _ping;
    }

protected:
    void incomingData(NP::AbstractData *pkg, const NP::HostAddress&  sender) {
        Q_UNUSED(sender);

        auto ping = dynamic_cast<NP::Ping*>(pkg);
        if (ping)
            _ping.copyFrom(ping);
    }

private:
    NP::Ping _ping;
};

class BadTstClient: public NP::DataBaseNode {

};

BaseNodeTest::BaseNodeTest() {
    _client1 = new TestingBaseClient();
    _client2 = new TestingBaseClient();

    _server = new NP::DataBaseNode();

}

BaseNodeTest::~BaseNodeTest() {
    delete _client1;
    delete _client2;
    delete _server;

}

void BaseNodeTest::test() {
    QVERIFY(testICtypto());
    QVERIFY(dbTest());
    QVERIFY(powerTest());
    QVERIFY(connectNetworkTest());
    QVERIFY(securityTest());

}

template<class Crypto>
bool validationCrypto() {
    // create crypto oject
    auto crypto = new NP::KeyStorage(new Crypto());


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
    crypto = new NP::KeyStorage(new Crypto());
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

    crypto = new NP::KeyStorage(new Crypto());
    if (!crypto->initDefaultStorageLocation()) {
        delete crypto;
        return false;
    }

    auto lastKeys = crypto->getNextPair("key2", RAND_KEY, 0);
    return lastKeys == ThisIsKey2;
}

bool BaseNodeTest::testICtypto() {
    // check
    if (!validationCrypto<NP::QSecretRSA2048>()) {
        return false;
    }

    return true;
}

bool BaseNodeTest::powerTest() {
    auto _nodeAPtr = new NP::DataBaseNode();

    if (!_nodeAPtr->run(TEST_LOCAL_HOST, TEST_PORT, "powerTest")) {
        return false;
    };

    delete _nodeAPtr;

    return true;
}

bool BaseNodeTest::dbTest() {
    auto node = new BaseNodeUnitTests;

    if (!node->test()) {
        return false;
    }

    delete node;

    return true;
}

bool BaseNodeTest::connectNetworkTest() {

    auto client1 = dynamic_cast<NP::DataBaseNode*>(_client1);
    auto client2 = dynamic_cast<NP::DataBaseNode*>(_client2);
    auto server = dynamic_cast<NP::DataBaseNode*>(_server);

    if (!server->run(TEST_LOCAL_HOST, TEST_PORT, "ServerDataBaseNode")) {
        return false;
    }

    auto addNodeRequest = [client1, client2]() {
        client1->addNode(NP::HostAddress(TEST_LOCAL_HOST, TEST_PORT));
        client2->addNode(NP::HostAddress(TEST_LOCAL_HOST, TEST_PORT));

        return true;
    };

    auto checkNode = [server](){
        return server->connectionsCount() == 2;
    };

    if (!funcPrivateConnect(addNodeRequest, checkNode)) {
        return false;
    }

    // need to wait for add node

    auto request = [client1, client2]() {
        return client1->ping(NP::HostAddress(TEST_LOCAL_HOST, TEST_PORT)) &&
               client2->ping(NP::HostAddress(TEST_LOCAL_HOST, TEST_PORT));
    };

    auto tstclient1 = dynamic_cast<TestingBaseClient*>(client1);
    auto tstclient2 = dynamic_cast<TestingBaseClient*>(client1);

    auto check = [tstclient1, tstclient2]() {
        return tstclient1->getPing().ansver() &&
               tstclient2->getPing().ansver();
    };

    return funcPrivateConnect(request, check);

}

bool BaseNodeTest::securityTest() {



    return true;
}



