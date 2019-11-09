#include <QtTest>

#include <thread>
#include <quasarapp.h>
#include <QCoreApplication>
#include <QCryptographicHash>
#include <networkprotocol.h>
#include <abstractdata.h>
#include <badrequest.h>
#include <package.h>
#include <basenode.h>
#include <client.h>

#include "testutils.h"

#define TEST_LOCAL_HOST "127.0.0.1"
#define TEST_PORT 1234

class testProtockol : public QObject
{
    Q_OBJECT

private:


public:
    testProtockol();

    ~testProtockol();

private slots:
    void initTestCase();
    void testPakageData();
    void testBaseNode();
    void testUser();

};

testProtockol::testProtockol() {
    QuasarAppUtils::Params::setArg("verbose", 3);

}

testProtockol::~testProtockol() {

}

void testProtockol::initTestCase() {
}

void testProtockol::testPakageData() {
    NetworkProtocol::BadRequest bad;
    NetworkProtocol::BadRequest bad1;
    NetworkProtocol::BadRequest bad2;

    QVERIFY(bad.cmd() == bad1.cmd() && bad.cmd() == bad2.cmd());

    NetworkProtocol::Package pkg;

    QVERIFY(bad.toPackage(pkg));

    NetworkProtocol::BadRequest res(pkg);

    QVERIFY(bad.cmd() == res.cmd());

}

void testProtockol::testBaseNode() {
    NetworkProtocol::BaseNode node, node2;

    const int port1 = TEST_PORT + 1;
    const int port2 = TEST_PORT + 2;

    QVERIFY(node.run(TEST_LOCAL_HOST, port1));
    QVERIFY(node2.run(TEST_LOCAL_HOST, port2));

    node.connectToHost(QHostAddress(TEST_LOCAL_HOST), port2);
    QVERIFY(node2.waitForNewConnection(1000));

    QVERIFY(node.getWorkState().getConnectionCount() == 1);

    node2.stop();

}

void testProtockol::testUser() {
    NetworkProtocol::BaseNode server;
    QVERIFY(server.run(TEST_LOCAL_HOST, TEST_PORT));
    NetworkProtocol::Client client(QHostAddress(TEST_LOCAL_HOST), TEST_PORT);

    QVERIFY(TestUtils::connectFunc(client, TEST_LOCAL_HOST, TEST_PORT));

    QVERIFY(TestUtils::loginFunc(client, "user", "123", true, true));
    QVERIFY(TestUtils::loginFunc(client, "user", "124", true, false));

}


QTEST_APPLESS_MAIN(testProtockol)

#include "tst_testsnakeserver.moc"
