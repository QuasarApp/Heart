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
#define TEST_PORT 27777

class testProtockol : public QObject
{
    Q_OBJECT

private:


public:
    testProtockol();

    void connectTest(NP::Client *cli, NP::BaseNode *serv);
    void testLogin(NP::Client *cli);

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

void testProtockol::connectTest(NP::Client *cli, NP::BaseNode *serv) {
    QVERIFY(serv->run(TEST_LOCAL_HOST, TEST_PORT));
    QVERIFY(TestUtils::connectFunc(cli, TEST_LOCAL_HOST, TEST_PORT));
}


void testProtockol::testLogin(NP::Client* cli) {
    QVERIFY(TestUtils::loginFunc(cli, "user", "123", true, true));
    QVERIFY(TestUtils::loginFunc(cli, "user", "124", true, false));
}

testProtockol::~testProtockol() {

}

void testProtockol::initTestCase() {
}

void testProtockol::testPakageData() {
    NP::BadRequest bad("Test");
    NP::BadRequest bad1;
    NP::BadRequest bad2;

    QVERIFY(bad.cmd() == bad1.cmd() && bad.cmd() == bad2.cmd());

    NP::Package pkg;

    QVERIFY(bad.toPackage(pkg));

    NP::BadRequest res(pkg);

    QVERIFY(bad.cmd() == res.cmd());
    QVERIFY(bad.err() == res.err());

}

void testProtockol::testBaseNode() {
    NP::BaseNode node, node2;

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
    int argc =0;
    char * argv[] = {nullptr};

    QCoreApplication app(argc, argv);

    QTimer::singleShot(0, [&app, this]() {

        NP::RatingUserNode *server = new NP::RatingUserNode();
        NP::Client * client = new NP::Client(QHostAddress(TEST_LOCAL_HOST), TEST_PORT);

        connectTest(client, server);
        testLogin(client);

        delete server;
        delete client;

        app.exit(0);
    });

    app.exec();


}


QTEST_APPLESS_MAIN(testProtockol)

#include "tst_testsnakeserver.moc"
