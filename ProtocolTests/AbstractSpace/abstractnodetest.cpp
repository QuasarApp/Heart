#include "abstractnodetest.h"
#include "testutils.h"

#include <abstractnode.h>
#include <ping.h>

class TestingClient: public NP::AbstractNode {


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

AbstractNodeTest::AbstractNodeTest() {
    _nodeA = new NP::AbstractNode();
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
        return _nodeB->ping(QHostAddress(TEST_LOCAL_HOST));
    };

    auto client = dynamic_cast<TestingClient*>(_nodeB);

    auto check = [client](){
        return client->getPing().ansver();
    };

    return funcPrivateConnect(request, check);
}
