#ifndef ITEST_H
#define ITEST_H

/**
 * @brief The iTest class is simle interface for testing.
 * This calass have only one method "Test" for run test functions.
 */
class iTest
{
public:
    iTest() = default;
    virtual ~iTest() = default;

    /**
     * @brief test run tests of object.
     * @return true if test finished successful
     */
    virtual bool test() = 0;
};

#endif // ITEST_H
