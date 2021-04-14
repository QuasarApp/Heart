/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/
#include "databasenodeunittests.h"

#include "databasenodetesttemplate.h"

#include <asyncsqldbwriter.h>
#include <sqlitedbcache.h>
#include "sqldb.h"
#include <networkmember.h>

// This define create a simple class based on the BASE class and use the CHECHE and WRITER like a default cache and default writer objects.
#define TEST_CASE(NAME, BASE, MEMBER, CACHE, WRITER) \
    class NAME: public DataBaseNodeUnitTestTemplate< \
            BASE, MEMBER, CACHE, WRITER> { \
    };

TEST_CASE(Case0, QH::DataBaseNode, QH::PKG::NetworkMember, QH::SqlDBCache, QH::AsyncSqlDBWriter)
TEST_CASE(Case1, QH::DataBaseNode, QH::PKG::NetworkMember, QH::SQLiteDBCache, QH::AsyncSqlDBWriter)
TEST_CASE(Case2, QH::DataBaseNode, QH::PKG::NetworkMember, QH::SqlDB, QH::AsyncSqlDBWriter)


template <class T>
void testCase(const T& t) {
    const_cast<T&>(t).test();
}

#define RUN_TEST_CASE(TYPE) \
    { \
    TYPE *tst = new TYPE(); \
    testCase(*tst); \
    tst->softDelete(); \
    }

void DataBaseNodeUnitTests::test() {
    RUN_TEST_CASE(Case0)
    RUN_TEST_CASE(Case1)
    RUN_TEST_CASE(Case2)

}
