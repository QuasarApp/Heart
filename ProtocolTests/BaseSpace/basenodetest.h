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
    NP::AbstractNode *_nodeC = nullptr;

    /**
     * @brief connectNetworkTest
     *  this test check nodes connections greatThen 3 node
     * @return
     */
    bool connectNetworkTest();

    /**
     * @brief transportDataTest
     *  this test create a small network and sending data to next route : A >> B >> C and C >> B >> A
     * @return
     */
    bool transportDataTest();

    /**
     * @brief performanceTest
     *  this test crate a big network from 100 or biger nodes count and send data for all nodes of network.
     * @return
     */
    bool performanceTest();

    /**
     * @brief powerTest - this test create big network and create not valid nodes. After created the network a not valid nodes try conquer network.
     * @return
     */
    bool powerTest();

};

#endif // BASENODETEST_H
