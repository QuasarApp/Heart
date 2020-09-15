#ifndef ABSTRACTNODETEST_H
#define ABSTRACTNODETEST_H
#include "test.h"
#include "testutils.h"

#include <QtTest>
#include <abstractnode.h>


class AbstractNodeTest: public Test, protected TestUtils
{
public:
    AbstractNodeTest();
    ~AbstractNodeTest();

    void test();

private:
    QH::AbstractNode *_nodeA = nullptr;
    QH::AbstractNode *_nodeB = nullptr;

    bool connectTest();
    bool sendDataTest();

    /**
     * @brief testICtypto - testing ictypto class
     * @return
     */
    bool testICtypto();
};

#endif // ABSTRACTNODETEST_H
