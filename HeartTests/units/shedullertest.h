//#
//# Copyright (C) 2021-2024 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#ifndef SHEDULLERTEST_H
#define SHEDULLERTEST_H

#include "test.h"
#include "testutils.h"

#include <QtTest>

class ShedullerTest: public Test, protected TestUtils
{
public:
    ShedullerTest();

    void test() override;

private:
    void testSingleMode();
    void testRepeatMode();
    void testTimePointMode();
};

#endif // SHEDULLERTEST_H
