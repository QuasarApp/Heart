#ifndef NETWORKTESTUTILS_H
#define NETWORKTESTUTILS_H

#include <basetestutils.h>
namespace NP {
class NetworkNode;
class BaseId;
}

class NetworkTestUtils: public BaseTestUtils
{
public:
    NetworkTestUtils();
    ~NetworkTestUtils();

    bool deployNewNode(NP::NetworkNode* node) const;
    QHash<NP::BaseId, NP::NetworkNode*> generateNetworkNode(int count) const;

    NP::NetworkNode *initNewNode() const;

protected:
    const NP::NetworkNode* getCoreNode();
private:
    int nextPort() const;

    NP::NetworkNode* coreNode = nullptr;
};

#endif // NETWORKTESTUTILS_H
