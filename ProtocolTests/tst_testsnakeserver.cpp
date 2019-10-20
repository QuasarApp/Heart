#include <QtTest>

#include <thread>
#include <quasarapp.h>
#include <QCoreApplication>
#include <QCryptographicHash>
#include <clientprotocol.h>

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
    void testAbstractNode();

};

testSankeServer::testSankeServer() {
    QuasarAppUtils::Params::setArg("verbose", 3);

}

testSankeServer::~testSankeServer() {

}

void testSankeServer::initTestCase() {
    //    ClientProtocol::initClientProtockol();
}

void testSankeServer::testAbstractNode() {
    ClientProtocol::AbstractNode node;
    node.run(TEST_LOCAL_HOST, TEST_PORT);

}


QTEST_APPLESS_MAIN(testSankeServer)

#include "tst_testsnakeserver.moc"
