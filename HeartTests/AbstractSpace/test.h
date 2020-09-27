/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#ifndef TEST_H
#define TEST_H

#define TEST_LOCAL_HOST "127.0.0.1"
#define TEST_PORT 27777

class Test
{
public:
    Test() = default;
    virtual ~Test() = default;
    virtual void test() = 0;
};

#endif // TEST_H
