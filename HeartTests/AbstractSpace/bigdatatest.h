/*
 * Copyright (C) 2018-2022 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#ifndef BIGDATATEST_H
#define BIGDATATEST_H

#include "test.h"
#include "testutils.h"

#include <QtTest>
#include <abstractnode.h>



class BigDataTest: public Test, protected TestUtils
{
public:
    BigDataTest();
    ~BigDataTest();

    void test();

private:
    QH::AbstractNode *_nodeA = nullptr;
    QH::AbstractNode *_nodeB = nullptr;

    bool connectTest();
    bool sendDataTest();
};

#endif // BIGDATATEST_H
