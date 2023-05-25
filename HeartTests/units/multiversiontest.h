/*
 * Copyright (C) 2023-2023 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef MULTIVERSIONTEST_H
#define MULTIVERSIONTEST_H


#include "test.h"
#include "testutils.h"

#include <QtTest>
#include <abstractnode.h>

/**
 * @brief The MultiVersionTest class test work of packages with different versions.
 */
class MultiVersionTest: public Test, protected TestUtils
{
public:
    MultiVersionTest();
    void test();

};

#endif // MULTIVERSIONTEST_H
