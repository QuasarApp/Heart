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
    QVERIFY(connectTest());
    QVERIFY(sendDataTest());

}

bool BaseNodeTest::connectTest() {

    _nodeA->listen(QHostAddress(TEST_LOCAL_HOST), TEST_PORT);

    return connectFunc(_nodeB, TEST_LOCAL_HOST, TEST_PORT);
}

bool BaseNodeTest::sendDataTest() {

    auto request = [this](){
        return _nodeB->ping(QHostAddress(TEST_LOCAL_HOST));
    };

    auto client = dynamic_cast<TestingBaseClient*>(_nodeB);

    auto check = [client](){
        return client->getPing().ansver();
    };

    return funcPrivateConnect(request, check);
}
