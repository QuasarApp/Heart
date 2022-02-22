/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#include <QtTest>

#if HEART_BUILD_LVL >= 0
#include "abstractnodetest.h"
#include <shedullertest.h>
#include <bigdatatest.h>
#include <ecdsaauthtest.h>
#endif
#if HEART_BUILD_LVL >= 1
#include <basenodetest.h>
#include <singleservertest.h>
#include <upgradedatabasetest.h>
#endif
#if HEART_BUILD_LVL >= 2
#include <networknodetest.h>
#endif

#define TestCase(name, testClass) \
    void name() { \
    initTest(new testClass()); \
    }

class testProtockol : public QObject
{
    Q_OBJECT
public:
    testProtockol();

    ~testProtockol();

private slots:
    // BEGIN TESTS CASES
#if HEART_BUILD_LVL >= 0

    TestCase(abstractNodeTest, AbstractNodeTest)
    TestCase(bigDataTest, BigDataTest);
    TestCase(shedullerTest, ShedullerTest);

#ifdef USE_HEART_SSL
    TestCase(ecdsaAuthTest, ECDSAAuthTest);
#endif

#endif
#if HEART_BUILD_LVL >= 1
    TestCase(baseNodeTest, BaseNodeTest)
    TestCase(singleNodeTest, SingleServerTest)
    TestCase(upgradeDataBaseTest, UpgradeDataBaseTest)
#endif
#if HEART_BUILD_LVL >= 2
    TestCase(networkNodeTest, NetworkNodeTest)
#endif

    // END TEST CASES

private:
    void initTest(Test* test);

    QCoreApplication *_app = nullptr;

};

testProtockol::testProtockol() {

    QH::init();

    // init xample unit test
    int argc =0;
    char * argv[] = {nullptr};

    _app = new QCoreApplication(argc, argv);
    QCoreApplication::setApplicationName("testHeart");
    QCoreApplication::setOrganizationName("QuasarApp");

    auto path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    QDir(path).removeRecursively();
}

testProtockol::~testProtockol() {
    _app->exit(0);
    delete _app;
}

void testProtockol::initTest(Test *test) {
    QTimer::singleShot(0, this, [this, test]() {
        test->test();
        delete test;
        _app->exit(0);
    });

    _app->exec();
}

QTEST_APPLESS_MAIN(testProtockol)

#include "tst_testprotockol.moc"
