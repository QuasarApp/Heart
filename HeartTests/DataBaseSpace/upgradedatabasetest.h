#ifndef UPGRADEDATABASETEST_H
#define UPGRADEDATABASETEST_H
#include "test.h"
#include "basetestutils.h"

class UpgradeDataBaseTest: public Test, protected BaseTestUtils
{
public:
    UpgradeDataBaseTest();

    // Test interface
public:
    void test();
};

#endif // UPGRADEDATABASETEST_H
