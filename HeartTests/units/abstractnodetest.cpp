/*
 * Copyright (C) 2018-2025 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#include "abstractnodetest.h"
#include "testutils.h"

#include <abstractnode.h>
#include <ping.h>

#define LOCAL_TEST_PORT TEST_PORT + 1

class TestingClient: public QH::AbstractNode {

    // AbstractNode interface
public:
    const QH::PKG::Ping& getPing() const {
        return _ping;
    }

    NodeType nodeType() const override {
        return NodeType::Node;
    };

protected slots:
    void receivePing(const QSharedPointer<QH::PKG::Ping>& ping) override {
        _ping = *ping;
    };

private:
    QH::PKG::Ping _ping;
};

AbstractNodeTest::AbstractNodeTest() {
    _nodeA = new TestingClient();
    _nodeB = new TestingClient();
}

AbstractNodeTest::~AbstractNodeTest() {
    _nodeA->softDelete();
    _nodeB->softDelete();
}

void AbstractNodeTest::test() {
    QVERIFY(connectTest());
    QVERIFY(sendDataTest());
}

bool AbstractNodeTest::connectTest() {

    if (!_nodeA->run(TEST_LOCAL_HOST, LOCAL_TEST_PORT)) {
        return false;
    }

    return connectFunc(_nodeB, TEST_LOCAL_HOST, LOCAL_TEST_PORT);
}

bool AbstractNodeTest::sendDataTest() {

    auto request = [this](){
        return _nodeB->ping(QH::HostAddress(TEST_LOCAL_HOST, LOCAL_TEST_PORT));
    };

    auto client = dynamic_cast<TestingClient*>(_nodeB);

    auto check = [client](){
        return client->getPing().ansver();
    };

    return funcPrivateConnect(request, check);
}

