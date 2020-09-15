#ifndef NETWORKTESTUTILS_H
#define NETWORKTESTUTILS_H

#include <basetestutils.h>
namespace QH {
class NetworkNode;
class BaseId;
}

class NetworkTestUtils: public BaseTestUtils
{
public:
    NetworkTestUtils();
    ~NetworkTestUtils();

    bool deployNewNode(QH::NetworkNode* node) const;
    QHash<QH::BaseId, QH::NetworkNode*> generateNetworkNode(int count) const;

    QH::NetworkNode *initNewNode() const;

protected:
    const QH::NetworkNode* getCoreNode();
private:
    int nextPort() const;

    QH::NetworkNode* coreNode = nullptr;
};

#endif // NETWORKTESTUTILS_H
