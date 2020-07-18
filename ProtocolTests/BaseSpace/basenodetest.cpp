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
    _nodeA = new NP::BaseNode();
    _nodeB = new TestingBaseClient();
}

BaseNodeTest::~BaseNodeTest() {
    delete _nodeA;
    delete _nodeB;
}

void BaseNodeTest::test() {
    QVERIFY(connectNetworkTest());
    QVERIFY(transportDataTest());

    QVERIFY(performanceTest());
    QVERIFY(powerTest());

}

bool BaseNodeTest::connectNetworkTest() {

}

bool BaseNodeTest::transportDataTest() {

}

bool BaseNodeTest::performanceTest() {

}

bool BaseNodeTest::powerTest() {

}


