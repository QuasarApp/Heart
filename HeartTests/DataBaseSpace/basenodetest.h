/*
 * Copyright (C) 2018-2022 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#ifndef BASENODETEST_H
#define BASENODETEST_H
#include "basetestutils.h"
#include "test.h"
#include "testutils.h"

#include <QtTest>
#include <heart.h>


class BaseNodeTest: public Test, protected BaseTestUtils
{
public:
    BaseNodeTest();
    ~BaseNodeTest();

    void test();

private:
    QH::AbstractNode *_server = nullptr;
    QH::AbstractNode *_client1 = nullptr;
    QH::AbstractNode *_client2 = nullptr;


    /**
     * @brief powerTest - this test just create a new object of node and distruct it.
     *  check constructors and distructors of nodes objects.
     * @return true if the test finished successful
     */
    void powerTest();

    /**
     * @brief dbTest - test base functions of database of nodes.
     * @return true if test finished successful.
     */
    void dbTest();

    /**
     * @brief connectNetworkTest
     *  this test check nodes connections greatThen 3 node
     * @return
     */
    void connectNetworkTest();

    /**
     * @brief securityTest - this test create 2 clinet app and try get or change data of another client.
     * @return
     */
    void securityTest();

};

#endif // BASENODETEST_H
