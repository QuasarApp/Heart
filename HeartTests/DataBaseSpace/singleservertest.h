/*
 * Copyright (C) 2020-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#ifndef SINGLESERVERTESTS_H
#define SINGLESERVERTESTS_H
#include "basetestutils.h"
#include "test.h"
#include "testutils.h"
#include <QtTest>

/**
 * @brief The SingleServerTest class testing the QH::SingleServer and QH::SingleClient clases.
 * This test case check next situations:
 * 1. connect client to sercer
 * 2. signup client to server
 * 3. login client to server
 * 4. remove clent record from server.
 */
class SingleServerTest: public Test, protected BaseTestUtils
{
public:
    SingleServerTest();
    ~SingleServerTest();

    void test() override;

private:
    /**
     * @brief connectNetworkTest
     *  this test check nodes connections greatThen 3 node
     * @return
     */
    void connectNetworkTest();

    bool initTest();


    QH::AbstractNode *_server = nullptr;
    QH::AbstractNode *_client = nullptr;
};

#endif // SINGLESERVERTESTS_H
