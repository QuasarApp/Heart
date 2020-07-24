#ifndef BASETESTUTILS_H
#define BASETESTUTILS_H

#include <testutils.h>

class BaseTestUtils: public TestUtils
{
public:
    BaseTestUtils();

    bool waintForReceivePackage();
};

#endif // BASETESTUTILS_H
