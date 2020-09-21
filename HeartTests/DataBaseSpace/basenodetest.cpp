#include "basenodetest.h"
#include "basenodeunittests.h"
#include "testutils.h"

#include <databasenode.h>
#include <hostaddress.h>
#include <keystorage.h>
#include <ping.h>
#include <qsecretrsa2048.h>

class TestingBaseClient: public QH::DataBaseNode {


    // AbstractNode interface
public:
    const QH::PKG::Ping& getPing() const {
        return _ping;
    }

protected:
    void incomingData(QH::PKG::AbstractData *pkg, const QH::HostAddress&  sender) {
        Q_UNUSED(sender);

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
    delete _client1;
    delete _client2;
    delete _server;

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



