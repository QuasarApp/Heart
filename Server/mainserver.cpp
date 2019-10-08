#include "keysreactor.h"
#include "mainserver.h"
#include "playerdbdata.h"
#include "sqldbcache.h"
#include "websocketcontroller.h"
#include <spserver.h>
#include <cpserver.h>
#include <quasarapp.h>
#include <basenetworkobject.h>
#include <login.h>
#include <QCoreApplication>
#include <rsakeyspool.h>
#include <getitem.h>
#include <websocket.h>

QByteArray MainServer::generateTocket(const QString& gmail) const {

    return QCryptographicHash::hash((gmail + QString::number(rand())).toLatin1(),
                                    QCryptographicHash::Sha256);
}

QByteArray MainServer::registerPlayer(const ClientProtocol::Login& login,
                                const ClientProtocol::RSAKeyPair& rsa) const {

    if (!login.isValid()) {
        return {};
    }

    if (_db->getPlayerId(login.getGmail()) > -1) {
        return {};
    }

    PlayerDBData player;
    player.setGmail( login.getGmail());

    player.setPass(QRSAEncryption::decode(login.getHashPass(), rsa.priv, BASE_RSA_BITS));

    int id = _db->savePlayer(player);
    if (id < 0) {
        return {};
    }

    player = _db->getPlayer(id);
    player.setToken(generateTocket(login.getGmail()));
    player.setName(QString("Palyer %0").arg(id));

    if (!player.isValid()) {
        QuasarAppUtils::Params::verboseLog("register Player fail!",
                                           QuasarAppUtils::Warning);
        return {};
    }

    if (id != _db->savePlayer(player)) {
        return {};
    }

    return player.getToken();
}

QByteArray MainServer::loginPlayer(const ClientProtocol::Login& login,
                             const ClientProtocol::RSAKeyPair& rsa) const {
    if (!login.isValid()) {
        return {};
    }

    auto pass = QRSAEncryption::decode(login.getHashPass(), rsa.priv, BASE_RSA_BITS);

    if (_db->login(login.getGmail(), pass.toHex())) {
        return generateTocket(login.getGmail());

    }

    return {};
}

bool MainServer::restartSrver(const QString &ip, unsigned short port) {
    if (_serverDaemon->isListening()) {
        _serverDaemon->stop();
    }

    if (!_serverDaemon->run(ip, port)) {
        return false;
    }

    return true;
}

void MainServer::handleRequest(ClientProtocol::Header hdr,
                               const QByteArray& data,
                               const quint32 &addres) {

    Q_UNUSED(addres);


    switch (static_cast<ClientProtocol::Command>(hdr.command)) {
    case ClientProtocol::Command::Login: {

        ClientProtocol::Login loginData;
        loginData.fromBytes(data);

        ClientProtocol::RSAKeyPair keys;
        if (!_serverDaemon->getRSA(addres, keys)) {
            _serverDaemon->badRequest(addres, hdr);
            return;
        }

        QByteArray tocken;
        if (loginData.getRegisterNewUser()) {
            tocken = registerPlayer(loginData, keys);
            if (!tocken.size()) {
                _serverDaemon->badRequest(addres, hdr);
                return;
            }
        } else {
            tocken = loginPlayer(loginData, keys);

            if (!tocken.size()) {
                _serverDaemon->badRequest(addres, hdr);
                return;
            }
        }

        ClientProtocol::UpdatePlayerData tockenObj;
        tockenObj.setToken(tocken);
        tockenObj.setId(_db->getPlayerId(loginData.getGmail()));

        if (!_serverDaemon->sendResponse(&tockenObj, addres, &hdr)) {
            QuasarAppUtils::Params::verboseLog("responce not sendet",
                                               QuasarAppUtils::Warning);
            return;
        }

        break;
    }

    case ClientProtocol::Command::GameData: {
        break;
    }

    case ClientProtocol::Command::UpdatePlayerData: {
        ClientProtocol::UpdatePlayerData request;
        request.fromBytes(data);

        auto tocken = _serverDaemon->getToken(addres);

        if (!tocken.isEmpty() && tocken == request.getToken()) {
            auto player = _db->getPlayer(request.id());
            _serverDaemon->sendResponse(&player, addres, &hdr);

        }

        _serverDaemon->badRequest(addres, hdr);
        break;
    }

    case ClientProtocol::Command::GetItem: {

        ClientProtocol::GetItem getRequest;
        getRequest.fromBytes(data);

        auto tocken = _serverDaemon->getToken(addres);

        if (!tocken.isEmpty() && tocken == getRequest.getToken()) {
            auto item = _db->getItem(getRequest.id());
            _serverDaemon->sendResponse(&item, addres, &hdr);

        }

        _serverDaemon->badRequest(addres, hdr);
        break;
    }

    case ClientProtocol::Command::WebSocket: {

        ClientProtocol::WebSocket websocket;
        websocket.fromBytes(data);

        if (websocket.isSubscribe()) {
            _websocketctrl->subscribe(addres, websocket.getCommand(),
                                        websocket.getObjectId());
        } else {
            _websocketctrl->unsubscribe(addres, websocket.getCommand(),
                                      websocket.getObjectId());
        }

        break;
    }

    default:
        _serverDaemon->badRequest(addres, hdr);
        break;
    }
}

void MainServer::handleTerminalRequest(QVariantMap obj) {

    auto command = static_cast<ServerProtocol::Command>(obj.value("command").toInt());
    QVariantMap res;

    switch (command) {
    case ServerProtocol::State: {
        res ["Work State"] = _serverDaemon->getWorkState();
        res ["Connections count"] = _serverDaemon->connectionState();

        auto banedList = _serverDaemon->baned();
        res ["Baned Addresses count"] = banedList.size();
        res ["Baned List"] = banedList;

        break;
    }
    case ServerProtocol::Ban: {
        auto address = obj.value("address").toUInt();

        _serverDaemon->ban(address);
        auto banedList = _serverDaemon->baned();
        res ["Baned List"] = banedList;

        break;
    }

    case ServerProtocol::Unban: {
        auto address = obj.value("address").toUInt();
        _serverDaemon->unBan(address);
        auto banedList = _serverDaemon->baned();

        res ["Baned List"] = banedList;
        break;
    }

    case ServerProtocol::Restart: {
        auto address = obj.value("address").toString();
        auto port = static_cast<quint16>(obj.value("port").toInt());

        if (!restartSrver(address, port)) {
            QuasarAppUtils::Params::verboseLog("server restart fail!");
        }

        res ["Work State"] = _serverDaemon->getWorkState();
        res ["Address"] = QString("%0:%1").
                arg(_serverDaemon->serverAddress().toString()).
                arg(_serverDaemon->serverPort());


        break;
    }

    case ServerProtocol::Stop: {

        res ["Res"] = "Server stoped!";
        _terminalPort->sendResponce(res, command);
        _serverDaemon->stop();
        QCoreApplication::processEvents();
        QCoreApplication::quit();
        return;

    }

    default:
        QuasarAppUtils::Params::verboseLog("server get undefined command!");
        res ["Error"] = "Server get undefined command!";
        break;
    }

    _terminalPort->sendResponce(res, command);
    return;
}

MainServer::MainServer(bool forceKeys ,QObject *ptr):
    QObject (ptr) {

    _keyReactor = new KeysReactor(forceKeys , this);
    _serverDaemon = new  ClientProtocol::Server(_keyReactor->getPool(), this);
    _websocketctrl = new WebSocketController(_serverDaemon, this);
    _terminalPort = new  ServerProtocol::Server(this);

    _db = new SqlDBCache();

    connect(_serverDaemon, &ClientProtocol::Server::incomingReques,
            this, &MainServer::handleRequest);

    connect(_terminalPort, &ServerProtocol::Server::incomingRequest,
            this, &MainServer::handleTerminalRequest);

    connect(_db, &SqlDBCache::sigItemChanged,
            _websocketctrl, &WebSocketController::handleItemChanged);

    connect(_db, &SqlDBCache::sigPlayerChanged,
            _websocketctrl, &WebSocketController::handlePlayerChanged);

    if (!ClientProtocol::initClientProtockol()){
        QuasarAppUtils::Params::verboseLog("clientProtocol no inited", QuasarAppUtils::Error);
    }

}

bool MainServer::run(const QString &ip, unsigned short port, const QString& db,
                     const QString& terminalServer) {

    if (!_db->initDb((db.size())? db: DEFAULT_DB_PATH)) {
        QuasarAppUtils::Params::verboseLog("init db fail!", QuasarAppUtils::Error);
        return false;
    }

    if (!_terminalPort->run((terminalServer.isEmpty())? DEFAULT_SERVER : terminalServer, true)) {
        QuasarAppUtils::Params::verboseLog("run termonal fail!", QuasarAppUtils::Error);
        return false;
    }

    if (!restartSrver(ip.isEmpty()? LOCAL_SNAKE_SERVER: ip,
                      port ? port : DEFAULT_SNAKE_PORT)) {
        QuasarAppUtils::Params::verboseLog("restart server fail", QuasarAppUtils::Error);
        return false;
    }

    return true;

}

MainServer::~MainServer() {
}
