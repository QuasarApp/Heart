#ifndef ABSTRACTNODETEST_H
#define ABSTRACTNODETEST_H
#include "test.h"
#include "testutils.h"

#include <QtTest>
#include <networkprotocol.h>


class AbstractNodeTest: public Test, protected TestUtils
{
public:
    AbstractNodeTest();
    ~AbstractNodeTest();

    void test();

private:
    NP::AbstractNode *_nodeA = nullptr;
    NP::AbstractNode *_nodeB = nullptr;

    bool connectTest();
    bool sendDataTest();
};

#endif // ABSTRACTNODETEST_H