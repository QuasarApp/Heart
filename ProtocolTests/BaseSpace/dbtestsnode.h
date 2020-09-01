#ifndef DBTESTS_H
#define DBTESTS_H

#include <basenode.h>

/**
 * @brief The DbTestsNode class - this implementation of node gor testing database
 */
class DbTestsNode: public NP::BaseNode
{
public:
    DbTestsNode();
    /**
     * @brief test - test work database
     * @return true if database of node work is correctly.
     */
    bool test();
};

#endif // DBTESTS_H
