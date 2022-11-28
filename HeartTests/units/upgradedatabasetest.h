#ifndef UPGRADEDATABASETEST_H
#define UPGRADEDATABASETEST_H
#include "test.h"
#include "testutils.h"

class UpgradeDataBaseTest: public Test, protected TestUtils
{
public:
    UpgradeDataBaseTest();

    // Test interface
public:
    void test();
};

#endif // UPGRADEDATABASETEST_H
