/*
 * Copyright (C) 2022-2022 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef ECDSAAUTHTEST_H
#define ECDSAAUTHTEST_H

#include "test.h"
#include "testutils.h"

class ECDSAAuthTest:  public Test, protected TestUtils
{
public:
    ECDSAAuthTest();
    ~ECDSAAuthTest();

    void test();

};

#endif // ECDSAAUTHTEST_H
