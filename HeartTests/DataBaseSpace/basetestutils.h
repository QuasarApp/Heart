/*
 * Copyright (C) 2018-2022 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#ifndef BASETESTUTILS_H
#define BASETESTUTILS_H

#include <testutils.h>
namespace QH {
class DataBaseNode;
class NodeId;
}

class BaseTestUtils: public TestUtils
{
public:
    BaseTestUtils();
    ~BaseTestUtils();

};

#endif // BASETESTUTILS_H
