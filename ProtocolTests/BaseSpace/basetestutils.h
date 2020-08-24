#ifndef BASETESTUTILS_H
#define BASETESTUTILS_H

#include <testutils.h>
namespace NP {
class BaseNode;
class BaseId;
}

class BaseTestUtils: public TestUtils
{
public:
    BaseTestUtils();

    bool deployNewNode(NP::BaseNode* node) const;
    NP::BaseNode* getCoreNode();
    QHash<NP::BaseId, NP::BaseNode*> generateNetworkNode(int count) const;

    NP::BaseNode *initNewNode() const;

protected:
    const NP::BaseNode* getCoreNode() const;
private:
    int nextPort() const;

    NP::BaseNode* coreNode = nullptr;
};

#endif // BASETESTUTILS_H
