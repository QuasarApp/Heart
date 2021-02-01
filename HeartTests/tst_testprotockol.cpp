/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#include <QtTest>

#if HEART_BUILD_LVL >= 0
#include "abstractnodetest.h"
#endif
#if HEART_BUILD_LVL >= 1
#include <basenodetest.h>
#endif
#if HEART_BUILD_LVL >= 2
#include <networknodetest.h>
#endif


class testProtockol : public QObject
{
    Q_OBJECT

private:
    QList<Test*> _tests;


public:
    testProtockol();

    ~testProtockol();

private slots:
    void initTestCase();
    void unitTests();

};

testProtockol::testProtockol() {

    QH::init();

#if HEART_BUILD_LVL >= 0
    _tests.push_back(new AbstractNodeTest);
#endif
#if HEART_BUILD_LVL >= 1
    _tests.push_back(new BaseNodeTest);
#endif
#if HEART_BUILD_LVL >= 2
    _tests.push_back(new NetworkNodeTest);
#endif
}

testProtockol::~testProtockol() {

}

void testProtockol::initTestCase() {
}

void testProtockol::unitTests() {
    int argc =0;
    char * argv[] = {nullptr};

    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("TestQNP");
    QCoreApplication::setOrganizationName("QuasarApp");

    auto path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    QDir(path).removeRecursively();

    QTimer::singleShot(0, [this, &app]() {

        for (auto test : qAsConst(_tests) ) {
            test->test();
            delete test;
        }

        app.exit(0);
    });

    app.exec();
}

QTEST_APPLESS_MAIN(testProtockol)

#include "tst_testprotockol.moc"
