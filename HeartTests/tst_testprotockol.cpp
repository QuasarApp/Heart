#include <QtTest>

#if HEART_BUILD_LVL >= 0
#include "abstractnodetest.h"
#endif
#if HEART_BUILD_LVL >= 1
#include <basenodetest.h>
#include <networknodetest.h>
#endif
#if HEART_BUILD_LVL >= 2
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

    QTimer::singleShot(0, [&app, this]() {

        for (auto test : _tests ) {
            test->test();
            delete test;
        }

        app.exit(0);
    });

    app.exec();
}

QTEST_APPLESS_MAIN(testProtockol)

#include "tst_testprotockol.moc"
