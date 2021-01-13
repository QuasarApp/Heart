/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


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

};

#endif // ABSTRACTNODETEST_H
