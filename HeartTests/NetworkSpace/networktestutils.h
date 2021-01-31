/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#ifndef NETWORKTESTUTILS_H
#define NETWORKTESTUTILS_H

#include <basetestutils.h>
namespace QH {
class NetworkNode;
class NodeId;
}

class NetworkTestUtils: public BaseTestUtils
{
public:
    NetworkTestUtils();
    ~NetworkTestUtils();

    bool deployNewNode(QH::NetworkNode* node) const;
    QHash<QH::NodeId, QH::NetworkNode*> generateNetworkNode(int count) const;

    QH::NetworkNode *initNewNode() const;

protected:
    const QH::NetworkNode* getCoreNode();
private:
    int nextPort() const;

    QH::NetworkNode* coreNode = nullptr;
};

#endif // NETWORKTESTUTILS_H
