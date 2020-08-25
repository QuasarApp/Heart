#include "basetestutils.h"

#include <basenode.h>
#include "test.h"

BaseTestUtils::BaseTestUtils() {

}

NP::BaseNode *BaseTestUtils::initNewNode() const {
    int port = nextPort();
    QString name = (coreNode)? QString("TestNode-%0").arg(port): QString("CoreNode-%0").arg(port);

    auto node = new NP::BaseNode();

    if (!node->run(TEST_LOCAL_HOST, port, name)) {
        delete node;
        return nullptr;
    }

    return node;
}

const NP::BaseNode *BaseTestUtils::getCoreNode() {
    if (!coreNode)
        coreNode = initNewNode();


    return coreNode;
}

bool BaseTestUtils::deployNewNode(NP::BaseNode* node) const {

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

QHash<NP::BaseId, NP::BaseNode *>
BaseTestUtils::generateNetworkNode(int count) const {

    QHash<NP::BaseId, NP::BaseNode *> result;
    QSet<NP::BaseNode *> tmp;

    auto deinit = [&tmp]() {
        for (NP::BaseNode * node : tmp) {
            delete node;
        }
        tmp.clear();
    };

    for (int i = 0; i < count; ++i) {
        NP::BaseNode *tmpNode = initNewNode();
        if (tmpNode)
            tmp.insert(tmpNode);
    }

    if (tmp.size() != count) {
        deinit();
        return {};
    }

    for (NP::BaseNode *node: tmp) {
        if (!deployNewNode(node)) {
            deinit();
            return {};
        }
    }

    for (auto i : tmp) {
        result.insert(i->nodeId(), i);
    }

    result.insert(coreNode->nodeId(), coreNode);

    return result;
}

int BaseTestUtils::nextPort() const {
    static int port = 0;
    int baseTestPort = TEST_PORT + 1000;
    return baseTestPort + port++;
}
