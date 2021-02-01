/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#include "basenodetest.h"
#include "databasenodeunittests.h"
#include "testutils.h"

#include <databasenode.h>
#include <hostaddress.h>
#include <keystorage.h>
#include <ping.h>

class TestingBaseClient: public QH::DataBaseNode {

    Q_OBJECT

    // AbstractNode interface
public:
    const QH::PKG::Ping& getPing() const {
        return _ping;
    }

protected:
    void incomingData(QH::PKG::AbstractData *pkg, const QH::HostAddress&  sender) override {
        Q_UNUSED(sender)

        auto ping = dynamic_cast<QH::PKG::Ping*>(pkg);
        if (ping)
            _ping.setAnsver(ping->ansver());
    }

private:
    QH::PKG::Ping _ping;
};

class BadTstClient: public QH::DataBaseNode {

};

BaseNodeTest::BaseNodeTest() {
    _client1 = new TestingBaseClient();
    _client2 = new TestingBaseClient();

    _server = new QH::DataBaseNode();

}

BaseNodeTest::~BaseNodeTest() {
    _client1->softDelete();
    _client2->softDelete();
    _server->softDelete();
}

void BaseNodeTest::test() {
    QVERIFY(dbTest());
    QVERIFY(powerTest());
    QVERIFY(connectNetworkTest());
    QVERIFY(securityTest());
}

bool BaseNodeTest::powerTest() {
    auto _nodeAPtr = new QH::DataBaseNode();

    if (!_nodeAPtr->run(TEST_LOCAL_HOST, TEST_PORT, "powerTest")) {
        return false;
    }

    _nodeAPtr->softDelete();

    return true;
}

bool BaseNodeTest::dbTest() {
    auto node = new DataBaseNodeUnitTests;

    if (!node->test()) {
        return false;
    }

    delete node;

    return true;
}

bool BaseNodeTest::connectNetworkTest() {

    auto client1 = dynamic_cast<QH::DataBaseNode*>(_client1);
    auto client2 = dynamic_cast<QH::DataBaseNode*>(_client2);
    auto server = dynamic_cast<QH::DataBaseNode*>(_server);

    if (!server->run(TEST_LOCAL_HOST, TEST_PORT, "ServerDataBaseNode")) {
        return false;
    }

    auto addNodeRequest = [client1, client2]() {
        client1->addNode(QH::HostAddress(TEST_LOCAL_HOST, TEST_PORT));
        client2->addNode(QH::HostAddress(TEST_LOCAL_HOST, TEST_PORT));

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
        return client1->ping(QH::HostAddress(TEST_LOCAL_HOST, TEST_PORT)) &&
               client2->ping(QH::HostAddress(TEST_LOCAL_HOST, TEST_PORT));
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



#include "basenodetest.moc"
