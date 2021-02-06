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

TEST_CASE(Case0, QH::DataBaseNode, QH::PKG::NetworkMember, QH::SqlDBCache, QH::SqlDBWriter)
TEST_CASE(Case1, QH::DataBaseNode, QH::PKG::NetworkMember, QH::SqlDBCache, QH::AsyncSqlDbWriter)
TEST_CASE(Case2, QH::DataBaseNode, QH::PKG::NetworkMember, QH::SQLiteDBCache, QH::SqlDBWriter)
TEST_CASE(Case3, QH::DataBaseNode, QH::PKG::NetworkMember, QH::SQLiteDBCache, QH::AsyncSqlDbWriter)
TEST_CASE(Case4, QH::DataBaseNode, QH::PKG::NetworkMember, QH::SqlDB, QH::SqlDBWriter)
TEST_CASE(Case5, QH::DataBaseNode, QH::PKG::NetworkMember, QH::SqlDB, QH::AsyncSqlDbWriter)


template <class T>
bool testCase(const T& t) {
    return const_cast<T&>(t).test();
}

#define RUN_TEST_CASE(TYPE) \
    { \
    TYPE *tst = new TYPE(); \
    if (!testCase(*tst)) \
        return false; \
    tst->softDelete(); \
    }

bool DataBaseNodeUnitTests::test() {
    RUN_TEST_CASE(Case0)
    RUN_TEST_CASE(Case1)
    RUN_TEST_CASE(Case2)
    RUN_TEST_CASE(Case3)
    RUN_TEST_CASE(Case4)
    RUN_TEST_CASE(Case5)

    return true;
}
