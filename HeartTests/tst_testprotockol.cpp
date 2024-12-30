/*
 * Copyright (C) 2018-2025 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#include <QtTest>

#include "abstractnodetest.h"
#include <shedullertest.h>
#include <bigdatatest.h>
#include <upgradedatabasetest.h>
#include <multiversiontest.h>

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

    TestCase(abstractNodeTest, AbstractNodeTest)
    TestCase(bigDataTest, BigDataTest);
    TestCase(shedullerTest, ShedullerTest);

    TestCase(upgradeDataBaseTest, UpgradeDataBaseTest)
    TestCase(multiVersionTest, MultiVersionTest)


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
