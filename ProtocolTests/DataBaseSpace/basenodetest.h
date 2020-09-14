#ifndef BASENODETEST_H
#define BASENODETEST_H
#include "basetestutils.h"
#include "test.h"
#include "testutils.h"

#include <QtTest>
#include <networkprotocol.h>


class BaseNodeTest: public Test, protected BaseTestUtils
{
public:
    BaseNodeTest();
    ~BaseNodeTest();

    void test();

private:
    NP::AbstractNode *_server = nullptr;
    NP::AbstractNode *_client1 = nullptr;
    NP::AbstractNode *_client2 = nullptr;

    /**
     * @brief testICtypto - testing ictypto class
     * @return
     */
    bool testICtypto();

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
     * @brief securityTest - this test create 2 clinet app and try get or change data of another client.
     * @return
     */
    bool securityTest();

};

#endif // BASENODETEST_H
