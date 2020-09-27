/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


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

QH::NetworkNode *NetworkTestUtils::initNewNode() const {
    int port = nextPort();
    QString name = (coreNode)? QString("TestNode-%0").arg(port): QString("CoreNode-%0").arg(port);

    auto node = new QH::NetworkNode();

    if (!node->run(TEST_LOCAL_HOST, port, name)) {
        delete node;
        return nullptr;
    }

    return node;
}

const QH::NetworkNode *NetworkTestUtils::getCoreNode() {
    if (!coreNode)
        coreNode = initNewNode();


    return coreNode;
}

bool NetworkTestUtils::deployNewNode(QH::NetworkNode* node) const {

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

QHash<QH::BaseId, QH::NetworkNode *>
NetworkTestUtils::generateNetworkNode(int count) const {

    QHash<QH::BaseId, QH::NetworkNode *> result;
    QSet<QH::NetworkNode *> tmp;

    auto deinit = [&tmp]() {
        for (QH::NetworkNode * node : tmp) {
            delete node;
        }
        tmp.clear();
    };

    for (int i = 0; i < count; ++i) {
        QH::NetworkNode *tmpNode = initNewNode();
        if (tmpNode)
            tmp.insert(tmpNode);
    }

    if (tmp.size() != count) {
        deinit();
        return {};
    }

    for (QH::NetworkNode *node: tmp) {
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
