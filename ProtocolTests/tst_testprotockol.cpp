#include <QtTest>
#include "abstractnodetest.h"

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
    _tests.push_back(new AbstractNodeTest);
}

testProtockol::~testProtockol() {

}

void testProtockol::initTestCase() {
}

void testProtockol::unitTests() {
    int argc =0;
    char * argv[] = {nullptr};

    QCoreApplication app(argc, argv);

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
