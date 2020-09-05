#include "basenodetest.h"
#include "basenodeunittests.h"
#include "testutils.h"

#include <basenode.h>
#include <keystorage.h>
#include <ping.h>
#include <qsecretrsa2048.h>

class TestingBaseClient: public NP::BaseNode {


    // AbstractNode interface
public:
    const NP::Ping& getPing() const {
        return _ping;
    }

protected:
    void incomingData(NP::AbstractData *pkg, const NP::BaseId &sender) {
        Q_UNUSED(sender);

        auto ping = dynamic_cast<NP::Ping*>(pkg);
        if (ping)
            _ping.copyFrom(ping);
    }

private:
    NP::Ping _ping;
};

BaseNodeTest::BaseNodeTest() {
    _nodeA = new TestingBaseClient();
    _nodeB = new NP::BaseNode();
    _nodeC = new TestingBaseClient();

}

BaseNodeTest::~BaseNodeTest() {
    delete _nodeA;
    delete _nodeB;
    delete _nodeC;

}

void BaseNodeTest::test() {
    QVERIFY(testICtypto());
    QVERIFY(dbTest());
    QVERIFY(powerTest());
    QVERIFY(connectNetworkTest());
    QVERIFY(transportDataTest());

//    QVERIFY(performanceTest());
//    QVERIFY(securityTest());

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
    auto _nodeAPtr = new NP::BaseNode();

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
    int nodeAPort = TEST_PORT + 0;
    int nodeBPort = TEST_PORT + 1;
    int nodeCPort = TEST_PORT + 2;


    auto _nodeAPtr = dynamic_cast<NP::BaseNode*>(_nodeA);
    auto _nodeBPtr = dynamic_cast<NP::BaseNode*>(_nodeB);
    auto _nodeCPtr = dynamic_cast<NP::BaseNode*>(_nodeC);

    if (!_nodeAPtr->run(TEST_LOCAL_HOST, nodeAPort, "TestNodeA")) {
        return false;
    }
    if (!_nodeBPtr->run(TEST_LOCAL_HOST, nodeBPort, "TestNodeB")) {
        return false;
    };
    if (!_nodeCPtr->run(TEST_LOCAL_HOST, nodeCPort, "TestNodeC")) {
        return false;
    };

    auto nodeA = _nodeAPtr->nodeId();
    auto nodeB = _nodeBPtr->nodeId();
    auto nodeC = _nodeCPtr->nodeId();


    auto addNodeRequest = [_nodeAPtr, nodeBPort, nodeCPort, _nodeBPtr, nodeC]() {
        _nodeAPtr->addNode(NP::HostAddress(TEST_LOCAL_HOST, nodeBPort));
        _nodeBPtr->addNode(NP::HostAddress(TEST_LOCAL_HOST, nodeCPort));
        return true;
    };

    auto checkNode = [_nodeAPtr, _nodeBPtr](){
        return _nodeAPtr->confirmendCount() && _nodeBPtr->confirmendCount();
    };

    if (!funcPrivateConnect(addNodeRequest, checkNode)) {
        return false;
    }

    // need to wait for add node

    auto request = [_nodeAPtr, nodeC]() {
        return _nodeAPtr->ping(nodeC);
    };

    auto client = dynamic_cast<TestingBaseClient*>(_nodeAPtr);

    auto check = [client](){
        return client->getPing().ansver();
    };

    return funcPrivateConnect(request, check);

}

bool BaseNodeTest::transportDataTest() {
    auto coreNode = getCoreNode();

    auto network = generateNetworkNode(30);

    if (!(network.size() && coreNode->confirmendCount() == 30)) {
        return false;
    }

    return true;
}

bool BaseNodeTest::performanceTest() {
    return false;
}

bool BaseNodeTest::securityTest() {
    return false;
}



