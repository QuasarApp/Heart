#ifndef NETWORKNODETEST_H
#define NETWORKNODETEST_H
#include "networktestutils.h"
#include "test.h"
#include "testutils.h"

#include <QtTest>


class NetworkNodeTest: public Test, protected NetworkTestUtils
{
public:
    NetworkNodeTest();
    ~NetworkNodeTest();

    void test();

private:
    QH::AbstractNode *_nodeA = nullptr;
    QH::AbstractNode *_nodeB = nullptr;
    QH::AbstractNode *_nodeC = nullptr;



    /**
     * @brief powerTest - this test just create a new object of node and distruct it.
     *  check constructors and distructors of nodes objects.
     * @return true if the test finished successful
     */
    bool powerTest();

    /**
     * @brief dbTest - test base functions of database of nodes.
     * @return true if test finished successful.
     */
    bool dbTest();

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
     * @brief securityTest - this test create big network and create not valid nodes. After created the network a not valid nodes try conquer network.
     * @return
     */
    bool securityTest();

};

#endif // NETWORKNODETEST_H
