#ifndef DBTESTS_H
#define DBTESTS_H

#include <basenode.h>

/**
 * @brief The DbTestsNode class - this implementation of node gor testing database
 */
class BaseNodeUnitTests: public NP::BaseNode
{
public:
    BaseNodeUnitTests();
    /**
     * @brief test - test work database
     * @return true if database of node work is correctly.
     */
    bool test();

private:
    /**
     * @brief init - init database.
     * @return return true if test module initialized successful
     */
    bool init();

    /**
     * @brief testReadWrite - test save and get object functions
     * @return true if all test finished successful
     */
    bool testReadWrite();

    /**
     * @brief testUpdate - test update functions
     * @return true if all test finished successful.
     */
    bool testUpdate();

    /**
     * @brief testChangeTrust - this test check bad request responce for node
     * @return true if all test finished successful
     */
    bool testChangeTrust();
};

#endif // DBTESTS_H
