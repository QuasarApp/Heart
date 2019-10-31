#include <QtTest>

#include <thread>
#include <quasarapp.h>
#include <QCoreApplication>
#include <QCryptographicHash>
#include <clientprotocol.h>
#include <abstractdata.h>
#include <badrequest.h>
#include <package.h>
#include <basenode.h>

#include "testutils.h"

#define TEST_LOCAL_HOST "127.0.0.1"
#define TEST_PORT 1234

class testSankeServer : public QObject
{
    Q_OBJECT

private:


public:
    testSankeServer();

    ~testSankeServer();

private slots:
    void initTestCase();
    void testPakageData();
    void testBaseNode();

};

testSankeServer::testSankeServer() {
    QuasarAppUtils::Params::setArg("verbose", 3);

}

testSankeServer::~testSankeServer() {

}

void testSankeServer::initTestCase() {
    ClientProtocol::initClientProtockol();
}

void testSankeServer::testPakageData() {
    ClientProtocol::BadRequest bad;
    ClientProtocol::BadRequest bad1;
    ClientProtocol::BadRequest bad2;

    QVERIFY(bad.cmd() == bad1.cmd() && bad.cmd() == bad2.cmd());

    ClientProtocol::Package pkg;

    QVERIFY(bad.toPackage(pkg));

    ClientProtocol::BadRequest res(pkg);

    QVERIFY(bad.cmd() == res.cmd());

}

void testSankeServer::testBaseNode() {
    ClientProtocol::BaseNode node, node2;

    const int port1 = TEST_PORT + 1;
    const int port2 = TEST_PORT + 2;

    QVERIFY(node.run(TEST_LOCAL_HOST, port1));
    QVERIFY(node2.run(TEST_LOCAL_HOST, port2));

    node.connectToHost(QHostAddress(TEST_LOCAL_HOST), port2);
    QVERIFY(node2.waitForNewConnection(1000));

    QVERIFY(node.getWorkState().getConnectionCount() == 1);

    node2.stop();


}


QTEST_APPLESS_MAIN(testSankeServer)

#include "tst_testsnakeserver.moc"
