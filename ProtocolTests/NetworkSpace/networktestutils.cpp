#include "networktestutils.h"

#include <networknode.h>
#include "test.h"

NetworkTestUtils::NetworkTestUtils() {

}

NetworkTestUtils::~NetworkTestUtils() {
    if (coreNode) {
        delete coreNode;
    }
}

NP::NetworkNode *NetworkTestUtils::initNewNode() const {
    int port = nextPort();
    QString name = (coreNode)? QString("TestNode-%0").arg(port): QString("CoreNode-%0").arg(port);

    auto node = new NP::NetworkNode();

    if (!node->run(TEST_LOCAL_HOST, port, name)) {
        delete node;
        return nullptr;
    }

    return node;
}

const NP::NetworkNode *NetworkTestUtils::getCoreNode() {
    if (!coreNode)
        coreNode = initNewNode();


    return coreNode;
}

bool NetworkTestUtils::deployNewNode(NP::NetworkNode* node) const {

    if (!node)
        return false;

    if (coreNode) {

        auto addNodeRequest = [node, this]() {
            node->addNode(coreNode->address());
            return true;
        };

        auto checkNode = [node](){
            return node->confirmendCount();
        };

        if (!funcPrivateConnect(addNodeRequest, checkNode)) {
            delete node;
            return false;
        }
    }

    return true;
}

QHash<NP::BaseId, NP::NetworkNode *>
NetworkTestUtils::generateNetworkNode(int count) const {

    QHash<NP::BaseId, NP::NetworkNode *> result;
    QSet<NP::NetworkNode *> tmp;

    auto deinit = [&tmp]() {
        for (NP::NetworkNode * node : tmp) {
            delete node;
        }
        tmp.clear();
    };

    for (int i = 0; i < count; ++i) {
        NP::NetworkNode *tmpNode = initNewNode();
        if (tmpNode)
            tmp.insert(tmpNode);
    }

    if (tmp.size() != count) {
        deinit();
        return {};
    }

    for (NP::NetworkNode *node: tmp) {
        if (!deployNewNode(node)) {
            deinit();
            return {};
        }
    }

    for (auto i : tmp) {
        result.insert(i->nodeId(), i);
    }

    auto check = [this, count](){
        return coreNode->confirmendCount() == count;
    };

    if (!funcPrivateConnect(nullptr, check)) {
        deinit();
        return {};
    }

    result.insert(coreNode->nodeId(), coreNode);

    return result;
}

int NetworkTestUtils::nextPort() const {
    static int port = 0;
    int baseTestPort = TEST_PORT + 1000;
    return baseTestPort + port++;
}
