#include <QtTest>
#include <cp.h>
#include <sp.h>
#include <thread>
#include <quasarapp.h>
#include <QCoreApplication>
#include <QCryptographicHash>
#include <sqldbwriter.h>
#include <sqldbcache.h>

#include <snake.h>
#include <playerdbdata.h>
#include <mainserver.h>

#include "factorynetobjects.h"
#include "testutils.h"

// add necessary includes here

#define TEST_LOCAL_SERVER QString(DEFAULT_SERVER) + "Test"
#define TEST_SERVER_ADDRESS  LOCAL_SNAKE_SERVER
#define TEST_SERVER_PORT 27777

class testSankeServer : public QObject
{
    Q_OBJECT

private:
    void testPingServerProtockol(ServerProtocol::Client &cli);
    void testStateServerProtockol(ServerProtocol::Client &cli);
    void testBanServerProtockol(ServerProtocol::Client& cli);
    void testUnBanServerProtockol(ServerProtocol::Client& cli);
    void testRestartServerProtockol(ServerProtocol::Client& cli);
    void testStopServerProtockol(ServerProtocol::Client& cli);

    void testPingClientProtockol(ClientProtocol::Client& cli);
    void testLogin(ClientProtocol::Client& cli);
    void testUserData(ClientProtocol::Client& cli);
    void testGetItem(ClientProtocol::Client& cli);
    void testApplyData(ClientProtocol::Client& cli);
    void testWebSockets(ClientProtocol::Client& cli);
    void testBanLogin(ClientProtocol::Client &cli, ServerProtocol::Client &term);

    void testBaseSql();
    void testSqlCache();

public:
    testSankeServer();

    ~testSankeServer();

private slots:
    void initTestCase();
    void clianupTestCase();

    void testProtockols();
//    void testClientProtockol();
    void testSql();


};

testSankeServer::testSankeServer() {
    QuasarAppUtils::Params::setArg("verbose", 3);

}

testSankeServer::~testSankeServer() {

}

void testSankeServer::initTestCase() {
    ClientProtocol::initClientProtockol();
}

void testSankeServer::clianupTestCase() {

}

void testSankeServer::testPingServerProtockol(ServerProtocol::Client &cli) {


    bool isWork = false;
    bool received = false;

    QMetaObject::Connection m_connection;

    m_connection = QObject::connect(&cli, &ServerProtocol::Client::sigIncommingData,
                     [&isWork, &received, &m_connection] (const QVariantMap& map) {

        isWork = map["res"].toString() == "Pong";
        disconnect(m_connection);
        received = true;


    });

    QVERIFY(cli.ping());

    QVERIFY(cli.wait(received, 1000));

    QVERIFY(isWork);

}

void testSankeServer::testStateServerProtockol(ServerProtocol::Client& cli) {
    QVariantMap map;
    QVERIFY(TestUtils::getState(cli, map));

}

void testSankeServer::testBanServerProtockol(ServerProtocol::Client& cli) {

    bool isWork = false;
    bool received = false;

    QMetaObject::Connection m_connection;

    m_connection = QObject::connect(&cli, &ServerProtocol::Client::sigIncommingData,
                     [&isWork, &received, &m_connection] (const QVariantMap& map) {

        isWork = !map.contains("Error");
        received = true;
        disconnect(m_connection);
    });

    QVERIFY(!cli.ban(QHostAddress()));

    QVERIFY(cli.ban(QHostAddress("192.192.192.192")));
    QVERIFY(cli.wait(received, 1000));

    QVERIFY(isWork);

}

void testSankeServer::testUnBanServerProtockol(ServerProtocol::Client& cli)
{

    bool isWork = false;
    bool received = false;

    QMetaObject::Connection m_connection;

    m_connection = QObject::connect(&cli, &ServerProtocol::Client::sigIncommingData,
                     [&isWork, &received, &m_connection] (const QVariantMap& map) {

        isWork = !map.contains("Error");
        received = true;
        disconnect(m_connection);

    });

    QVERIFY(!cli.unBan(QHostAddress()));

    QVERIFY(cli.unBan(QHostAddress("192.192.192.192")));
    QVERIFY(cli.wait(received, 1000));

    QVERIFY(isWork);

}

void testSankeServer::testRestartServerProtockol(ServerProtocol::Client& cli) {

    bool isWork = false;
    bool received = false;

    QMetaObject::Connection m_connection;

    m_connection = QObject::connect(&cli, &ServerProtocol::Client::sigIncommingData,
                     [&isWork, &received, &m_connection] (const QVariantMap& map) {

        isWork =  !map.contains("Error");
        received = true;
        disconnect(m_connection);

    });

    QVERIFY(!cli.restart("lolo", 0));

    QVERIFY(!cli.restart("192.168.1.999", 0));
    QVERIFY(!cli.restart("192.168.1.99", 0));
    QVERIFY(!cli.restart("192.168.1.9", 0));

    QVERIFY(!cli.restart("-1.168.1.999", 77));
    QVERIFY(!cli.restart("192.168.-1.99", 7777));

    QVERIFY(cli.restart("127.168.1.9", 3456));

    cli.wait(received, 1000);
    QVERIFY(isWork);

    isWork = false;
    received = false;

    m_connection = QObject::connect(&cli, &ServerProtocol::Client::sigIncommingData,
                     [&isWork, &received, &m_connection] (const QVariantMap& map) {

        isWork = map.value("Address") == QString("%0:%1").
                arg(TEST_SERVER_ADDRESS).
                arg(TEST_SERVER_PORT) &&
                !map.contains("Error");
        received = true;
        disconnect(m_connection);

    });

    QVERIFY(cli.restart(TEST_SERVER_ADDRESS, TEST_SERVER_PORT));

    cli.wait(received, 1000);
    QVERIFY(isWork);

}

void testSankeServer::testStopServerProtockol(ServerProtocol::Client& cli) {

    bool isWork = false;
    bool received = false;

    QMetaObject::Connection m_connection;

    m_connection = QObject::connect(&cli, &ServerProtocol::Client::sigIncommingData,
                     [&isWork, &received, &m_connection] (const QVariantMap& map) {

        isWork = !map.contains("Error");
        received = true;
        disconnect(m_connection);

    });

    QVERIFY(cli.stop());

    cli.wait(received, 1000);

    QVERIFY(!received || isWork);
}

void testSankeServer::testPingClientProtockol(ClientProtocol::Client &cli) {

    bool isWork = false;
    bool received = false;

    QMetaObject::Connection m_connection;

    m_connection = QObject::connect(&cli, &ClientProtocol::Client::sigIncommingData,
                     [&isWork, &received, &m_connection] (const ClientProtocol::Command cmd,
                                    const QByteArray&) {

        isWork = cmd == ClientProtocol::Command::Ping;
        received = true;
        disconnect(m_connection);


    });

    QVERIFY(cli.ping());

    QVERIFY(TestUtils::wait(received, 1000));

    QVERIFY(isWork);

}

void testSankeServer::testLogin(ClientProtocol::Client &cli) {

    QVERIFY(TestUtils::loginFunc(cli, "Test@gmail.com", "testpass", true, false));

    QVERIFY(TestUtils::registerFunc(cli, "Test@gmail.com", "testpass", true, true));
    QVERIFY(TestUtils::registerFunc(cli, "Test@gmail.com", "testpass", true, false));

    QVERIFY(TestUtils::loginFunc(cli, "Test@gmail.com", "testpass", true, true));
    QVERIFY(TestUtils::loginFunc(cli, "Test@gmail.com", "testpass2", true, false));
    QVERIFY(TestUtils::loginFunc(cli, "Test@gmail.com", "testpass", true, true));

}

void testSankeServer::testBanLogin(ClientProtocol::Client &cli, ServerProtocol::Client &term) {

    QVariantMap state;
    TestUtils::getState(term, state);

    for (int i = 0; i < 25 && cli.isOnline(); i++) {
        QVERIFY(TestUtils::loginFunc(cli, "Test@gmail.com", "testpass2", true, false) == cli.isOnline());
    }

    TestUtils::getState(term, state);
    QVERIFY(state.value("Baned List").toStringList().contains("127.0.0.1"));
    QVERIFY(TestUtils::loginFunc(cli, "Test@gmail.com", "testpass", false, false));
    QVERIFY(TestUtils::loginFunc(cli, "Test123@gmail.com", "testpass2", false, false));

    TestUtils::unBanFunc(term, QHostAddress::LocalHost);

    TestUtils::getState(term, state);
    QVERIFY(!state.value("Baned List").toStringList().contains("127.0.0.1"));

    TestUtils::banFunc(term, QHostAddress::LocalHost);

    TestUtils::getState(term, state);
    QVERIFY(state.value("Baned List").toStringList().contains("127.0.0.1"));

    TestUtils::unBanFunc(term, QHostAddress::LocalHost);
    cli.reconnectToHost();

    TestUtils::reconnectFunc(cli);
    QVERIFY(TestUtils::loginFunc(cli, "Test@gmail.com", "testpass", true, true));
    QVERIFY(TestUtils::loginFunc(cli, "Test123@gmail.com", "testpass2", true, false));

}

void testSankeServer::testUserData(ClientProtocol::Client &cli) {

    cli._token = "";
    QVERIFY(!cli.updateData());

    cli._token = QCryptographicHash::hash("testtoken", QCryptographicHash::Sha256);
    QVERIFY(cli.updateData());
}

void testSankeServer::testGetItem(ClientProtocol::Client &cli) {
    cli._token = "";
    QVERIFY(!cli.getItem(1));

    cli._token = QCryptographicHash::hash("testtoken", QCryptographicHash::Sha256);
    QVERIFY(cli.getItem(1));
}

void testSankeServer::testApplyData(ClientProtocol::Client &cli) {

    cli._token = QCryptographicHash::hash("testtoken", QCryptographicHash::Sha256);

    QVERIFY(!cli.savaData(QList<int>()));

    QList<int> listData = {1};

    QVERIFY(cli.savaData(listData));

}

void testSankeServer::testWebSockets(ClientProtocol::Client &cli) {

    QVERIFY(cli.setSubscribe(ClientProtocol::Command::Player, true));
    QVERIFY(cli.setSubscribe(ClientProtocol::Command::Player, false));


}

void testSankeServer::testBaseSql() {
    SqlDBWriter db;
    QFile::remove("./test.db");

    bool init = db.initDb("./test.db");

    if (!init) {
        QFile::remove("./test.db");
    }

    QVERIFY(init);

    ClientProtocol::Snake snake;
    snake.setSpeed(10);
    snake.setSkillet(QList<float>() << 1);
    snake.setSnakeClass(0);


    // TEST ITEM

    ClientProtocol::Snake resSnake;

    QVERIFY(db.saveItem(&snake) < 0);
    snake.setId(0);
    int id = db.saveItem(&snake);

    QVERIFY(id == 0);

    QVERIFY(db.getItem(id).parse(resSnake));

    QVERIFY(snake.getSpeed() == resSnake.getSpeed());
    QVERIFY(snake.getSkillet() == resSnake.getSkillet());
    QVERIFY(snake.getSnakeClass() == resSnake.getSnakeClass());
    QVERIFY(snake.getClass() == resSnake.getClass());
    QVERIFY(snake.id() == resSnake.id());

    resSnake.setSnakeClass(10);
    QVERIFY(id == db.saveItem(Item(&resSnake)));

    ClientProtocol::Snake temp;
    QVERIFY(db.getItem(id).parse(temp));

    QVERIFY(temp.getSnakeClass() == 10);


    // TEST PLAYER

    PlayerDBData player = PlayerDBData();

    player.setMany(10);
    player.setLastOnline(1000);
    player.setOnlineTime(1001);
    player.setName("test");
    player.fromHexPass("FF");
    player.setGmail("test@gmail.com");
    player.setCureentSnake(0);

    QVERIFY(db.savePlayer(player) < 0);
    player.setId(0);

    QVERIFY(db.savePlayer(player) < 0);
    player.setCureentSnake(-1);
    id = db.savePlayer(player);
    QVERIFY(id == 0);

    QVERIFY(!db.saveowners(id, QSet<int>() << 1));
    QVERIFY(db.saveowners(id, QSet<int>() << 0));

    QSet<int> items;
    QVERIFY(db.getAllItemsOfPalyer(id, items));
    QVERIFY(items.contains(0));
    QVERIFY(items.size() == 1);


    player.setCureentSnake(0);
    id = db.savePlayer(player);

    auto resPlayer = db.getPlayer(id);
    QVERIFY(resPlayer.isValid());
    QVERIFY(player.getLastOnline() == resPlayer.getLastOnline());
    QVERIFY(player.getMany() == resPlayer.getMany());
    QVERIFY(player.getOnlineTime() == resPlayer.getOnlineTime());
    QVERIFY(player.getName() == resPlayer.getName());
    QVERIFY(player.getPass() == resPlayer.getPass());
    QVERIFY(player.getCureentSnake() == resPlayer.getCureentSnake());


    player.setCureentSnake(3);

    QVERIFY(db.savePlayer(player) < 0);
    player.setCureentSnake(0);
    player.setName("new");
    QVERIFY(db.savePlayer(player) == id);

    resPlayer = db.getPlayer(id);

    QVERIFY(resPlayer.getName() == "new");

}

void testSankeServer::testSqlCache() {
    SqlDBCache db;

    QFile::remove("./test2.db");

    bool init = db.initDb("./test2.db");

    if (!init) {
        QFile::remove("./test2.db");
    }

    QVERIFY(init);


    ClientProtocol::Snake snake;
    snake.setSpeed(10);
    snake.setSkillet(QList<float>() << 1);
    snake.setSnakeClass(0);


    // TEST ITEM

    ClientProtocol::Snake resSnake;

    int id = db.saveItem(&snake);

    QVERIFY(id == 0);
    snake.setId(id);

    QVERIFY(db.getItem(id).parse(resSnake));

    QVERIFY(snake.getSpeed() == resSnake.getSpeed());
    QVERIFY(snake.getSkillet() == resSnake.getSkillet());
    QVERIFY(snake.getSnakeClass() == resSnake.getSnakeClass());
    QVERIFY(snake.getClass() == resSnake.getClass());
    QVERIFY(snake.id() == resSnake.id());

    resSnake.setSnakeClass(10);
    QVERIFY(id == db.saveItem(Item(&resSnake)));

    ClientProtocol::Snake temp;
    QVERIFY(db.getItem(id).parse(temp));

    QVERIFY(temp.getSnakeClass() == 10);


    // TEST PLAYER

    PlayerDBData player = PlayerDBData();

    player.setMany(10);
    player.setLastOnline(1000);
    player.setOnlineTime(1001);
    player.setName("test");
    player.fromHexPass("FF");
    player.setGmail("test@gmail.com");
    player.setCureentSnake(0);

    QVERIFY(db.savePlayer(player) < 0);
    player.setId(0);

    QVERIFY(db.savePlayer(player) < 0);
    player.setCureentSnake(-1);
    id = db.savePlayer(player);
    QVERIFY(id == 0);

    QVERIFY(!db.getItem(id, 1));
    QVERIFY(db.getItem(id, 0));

    player.setCureentSnake(0);
    QVERIFY(db.savePlayer(player) == id);

    auto resPlayer = db.getPlayer(id);
    QVERIFY(resPlayer.isValid());
    QVERIFY(player.getLastOnline() == resPlayer.getLastOnline());
    QVERIFY(player.getMany() == resPlayer.getMany());
    QVERIFY(player.getOnlineTime() == resPlayer.getOnlineTime());
    QVERIFY(player.getName() == resPlayer.getName());
    QVERIFY(player.getPass() == resPlayer.getPass());
    QVERIFY(player.getCureentSnake() == resPlayer.getCureentSnake());


    player.setCureentSnake(3);

    QVERIFY(db.savePlayer(player) < 0);
    player.setCureentSnake(0);
    player.setName("new");
    QVERIFY(db.savePlayer(player) == id);

    resPlayer = db.getPlayer(id);

    QVERIFY(resPlayer.getName() == "new");


    PlayerDBData second_player = PlayerDBData();

    second_player.setMany(10);
    second_player.setLastOnline(1000);
    second_player.setOnlineTime(1001);
    second_player.setName("test2");
    second_player.setPass("test2");
    second_player.setGmail("test2@gmail.com");
    second_player.setCureentSnake(-1);
    second_player.setId(-1);

    QVERIFY(db.savePlayer(second_player) == 1);

    QVERIFY(db.moveItem(0, 1, 0));

    QSet<int> items;
    QVERIFY(db.getAllItemsOfPalyer(1, items));
    QVERIFY(items.contains(0));
    QVERIFY(items.size() == 1);

    db.updateInterval = 0;

    db.globalUpdateDataBasePrivate(0);

}

void testSankeServer::testSql() {
    testBaseSql();
    testSqlCache();
}

void testSankeServer::testProtockols() {

    int argc =0;
    char * argv[] = {nullptr};

    QCoreApplication app(argc, argv);

    auto serv = new MainServer(true, this);
    QVERIFY(serv->run(TEST_SERVER_ADDRESS, TEST_SERVER_PORT , "", TEST_LOCAL_SERVER));
    ServerProtocol::Client cliS(TEST_LOCAL_SERVER);
    ClientProtocol::Client cliC(TEST_SERVER_ADDRESS, TEST_SERVER_PORT);

    QVERIFY(TestUtils::wait(cliC.isOnline(), 1000));

    QTimer::singleShot(0, [this, &app, &cliS, &cliC]() {

        // test client protockol
        testPingClientProtockol(cliC);

        testLogin(cliC);
        testGetItem(cliC);
        testUserData(cliC);
        testApplyData(cliC);
        testWebSockets(cliC);
        testBanLogin(cliC, cliS);

        // test server protockiol
        testPingServerProtockol(cliS);
        testStateServerProtockol(cliS);
        testBanServerProtockol(cliS);
        testUnBanServerProtockol(cliS);
        testRestartServerProtockol(cliS);
        testStopServerProtockol(cliS);
        app.exit(0);
    });

    app.exec();

}

//void testSankeServer::testClientProtockol() {

//    auto serv = new ClientProtocol::Server(this);
//    QVERIFY(serv->run(TEST_SERVER_ADDRESS, TEST_SERVER_PORT));


//}

QTEST_APPLESS_MAIN(testSankeServer)

#include "tst_testsnakeserver.moc"
