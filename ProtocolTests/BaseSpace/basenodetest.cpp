#include "basenodetest.h"
#include "testutils.h"

#include <basenode.h>
#include <ping.h>

class TestingBaseClient: public NP::BaseNode {


    // AbstractNode interface
public:
    const NP::Ping& getPing() const {
        return _ping;
    }

protected:
    void incomingData(NP::AbstractData *pkg, const QHostAddress &sender) {
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
    QVERIFY(connectNetworkTest());
    QVERIFY(transportDataTest());

    QVERIFY(performanceTest());
    QVERIFY(powerTest());

}

bool BaseNodeTest::connectNetworkTest() {
    int nodeA = TEST_PORT + 0;
    int nodeB = TEST_PORT + 1;
    int nodeC = TEST_PORT + 2;


    _nodeA->listen(QHostAddress(TEST_LOCAL_HOST), nodeA);
    _nodeB->listen(QHostAddress(TEST_LOCAL_HOST), nodeB);
    _nodeC->listen(QHostAddress(TEST_LOCAL_HOST), nodeC);

    _nodeA->addNode(QHostAddress(TEST_LOCAL_HOST), nodeB);
    _nodeB->addNode(QHostAddress(TEST_LOCAL_HOST), nodeC);



    return false;
}

bool BaseNodeTest::transportDataTest() {
    return false;
}

bool BaseNodeTest::performanceTest() {
    return false;
}

bool BaseNodeTest::powerTest() {
    return false;
}


