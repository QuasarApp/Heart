#ifndef BASENODETEST_H
#define BASENODETEST_H
#include "test.h"
#include "testutils.h"

#include <QtTest>
#include <networkprotocol.h>


class BaseNodeTest: public Test, protected TestUtils
{
public:
    BaseNodeTest();
    ~BaseNodeTest();

    void test();

private:
    NP::AbstractNode *_nodeA = nullptr;
    NP::AbstractNode *_nodeB = nullptr;

    bool connectTest();
    bool sendDataTest();
};

#endif // BASENODETEST_H
