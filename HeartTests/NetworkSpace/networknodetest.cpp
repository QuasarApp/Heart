/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#include "networknodetest.h"
#include "networknodeunittests.h"
#include "testutils.h"

#include <networknode.h>
#include <keystorage.h>
#include <ping.h>
#include <qsecretrsa2048.h>
#include <keystoragetest.h>

class TestingNetworkClient: public QH::NetworkNode {


    // AbstractNode interface
public:
    const QH::PKG::Ping& getPing() const {
        return _ping;
    }

protected:
    void incomingData(QH::PKG::AbstractData *pkg, const QH::NodeId &sender) {
        Q_UNUSED(sender);

        auto ping = dynamic_cast<QH::PKG::Ping*>(pkg);
        if (ping)
            _ping.setAnsver(ping->ansver());
    }

private:
    QH::PKG::Ping _ping;
};

NetworkNodeTest::NetworkNodeTest() {
    _nodeA = new TestingNetworkClient();
    _nodeB = new QH::NetworkNode();
    _nodeC = new TestingNetworkClient();

}

NetworkNodeTest::~NetworkNodeTest() {
    delete _nodeA;
    delete _nodeB;
    delete _nodeC;

}

void NetworkNodeTest::test() {
    QVERIFY(dbTest());
    QVERIFY(powerTest());
    QVERIFY(connectNetworkTest());
    QVERIFY(transportDataTest());

//    QVERIFY(performanceTest());
//    QVERIFY(securityTest());

}

bool NetworkNodeTest::powerTest() {
    auto _nodeAPtr = new QH::NetworkNode();

    if (!_nodeAPtr->run(TEST_LOCAL_HOST, TEST_PORT, "powerTest")) {
        return false;
    };

    delete _nodeAPtr;

    return true;
}

bool NetworkNodeTest::dbTest() {
    auto node = new NetworkNodeUnitTests;

    if (!node->test()) {
        return false;
    }

    delete node;

    return true;
}

bool NetworkNodeTest::connectNetworkTest() {
    int nodeAPort = TEST_PORT + 0;
    int nodeBPort = TEST_PORT + 1;
    int nodeCPort = TEST_PORT + 2;


    auto _nodeAPtr = dynamic_cast<QH::NetworkNode*>(_nodeA);
    auto _nodeBPtr = dynamic_cast<QH::NetworkNode*>(_nodeB);
    auto _nodeCPtr = dynamic_cast<QH::NetworkNode*>(_nodeC);

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
        _nodeAPtr->addNode(QH::HostAddress(TEST_LOCAL_HOST, nodeBPort));
        _nodeBPtr->addNode(QH::HostAddress(TEST_LOCAL_HOST, nodeCPort));
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

    auto client = dynamic_cast<TestingNetworkClient*>(_nodeAPtr);

    auto check = [client](){
        return client->getPing().ansver();
    };

    return funcPrivateConnect(request, check);

}

bool NetworkNodeTest::transportDataTest() {
    auto coreNode = getCoreNode();

    auto network = generateNetworkNode(30);

    if (!(network.size() && coreNode->confirmendCount() == 30)) {
        return false;
    }

    return true;
}

bool NetworkNodeTest::performanceTest() {
    return false;
}

bool NetworkNodeTest::securityTest() {
    return false;
}

bool NetworkNodeTest::testICtypto() {
    // check
    if (!validationCrypto<QH::QSecretRSA2048>()) {
        return false;
    }

    return true;
}



