#ifndef DATABASENODETESTTEMPLATE_H
#define DATABASENODETESTTEMPLATE_H

#include "databasetestnode.h"
#include "templatedatabasenodeunittests.h"

#define DB_NODE_NAME "DatabaseTestNode"

template <class NODE, class MEMBER, class CACHE, class WRITER>

/**
 * @brief The DataBaseNodeUnitTestTemplate class This is template class for create tests.
 */
class DataBaseNodeUnitTestTemplate: public TemplateDataBaseNodeUnitTests<DataBaseTestNode<NODE, CACHE, WRITER>, MEMBER> {
public:
    DataBaseNodeUnitTestTemplate():
        TemplateDataBaseNodeUnitTests<DataBaseTestNode<NODE, CACHE, WRITER>, MEMBER>(DB_NODE_NAME) {}
protected:
    MEMBER *randomMember() const override {
        srand(time(nullptr));

        MEMBER * res = new MEMBER();
        res->setAuthenticationData(this->randomArray(64));
        res->setTrust(0);
        res->setName(this->randomArray(5).toHex());

        res->prepareToSend();

        return res;

    };
};

#endif // DATABASENODETESTTEMPLATE_H
