#include "client.h"
#include <QRegularExpression>
#include <QTcpSocket>
#include <QVariantMap>
#include <QDateTime>
#include <quasarapp.h>
#include <qrsaencryption.h>
#include <pubkey.h>
#include <websocket.h>
#include "factorynetobjects.h"
#include "gamedata.h"
#include "getitem.h"
#include "login.h"
#include "updateplayerdata.h"
#include <QHash>
#include <player.h>

#define SOLT "SNAKE"
namespace ClientProtocol {

Command Client::checkCommand(int sig, Command reqCmd, Type type) {

#define idx static_cast<quint8>(sig)

    auto expCmd = static_cast<Command>(
                _requestsMap[idx].value("expected",
                                        static_cast<quint8>(Command::Undefined)).toInt());

    if (expCmd == Command::Undefined ||
            expCmd != reqCmd ||
            type != Type::Responke) {

        return Command::Undefined;
    }

    _requestsMap[idx]["time"] = QDateTime::currentMSecsSinceEpoch();
    return expCmd;
}

void Client::updateStatuses(Command extCmd, Command cmd, Type type, const QByteArray& obj)
{
    setOnlineStatus(extCmd != Command::Undefined && type == Type::Responke);

    if (extCmd == Command::Login
             && type == Type::Responke) {

        UpdatePlayerData data;
        data.fromBytes(obj);
        bool validData = data.isValid();
        if (validData) {
            _token = data.getToken();

        }

        _currentUserId = data.id();

        setLoginStatus(cmd == Command::UpdatePlayerData && validData);
    }
}

bool Client::receiveData(const QByteArray &obj, Header hdr) {

    auto command = static_cast<Command>(hdr.command);
    auto requesCommand = static_cast<Command>(hdr.requestCommand);

    auto type = static_cast<Type>(hdr.type);

    if (command == Command::PubKey && !_rsaKey.size()) {
        PubKey data;       
        data.fromBytes(obj);
        return setRSAKey(data.getKey());;
    }

    if (type != Type::Stream) {
        auto expectedCommand = checkCommand(hdr.sig, requesCommand, type);

        if (expectedCommand == Command::Undefined) {
            QuasarAppUtils::Params::verboseLog("wrong sig of package");
            return false;
        }

        updateStatuses(expectedCommand, command, type, obj);
    } else if (_subscribe.contains(hdr.command)) {
        _subscribe[hdr.command] = true;
    } else {
        return false;
    }

    emit sigIncommingData(static_cast<Command>(hdr.command), obj);

    return true;
}

bool Client::setRSAKey(const QByteArray& key) {
    bool newStatus = QRSAEncryption::isValidRsaKey(key);
    setOnlineStatus(newStatus);

    if (newStatus) {
        _rsaKey = key;
    }

    return newStatus;
}

void Client::setLoginStatus(bool newStatus) {
    if (newStatus != _logined) {
        _logined = newStatus;
        emit loginChanged(_logined);
    }
}

void Client::setOnlineStatus(bool newOnline) {
    if (newOnline != _online) {
        _online = newOnline;
        emit onlineChanged(_online);
        if (!_online) {
            _rsaKey = "";
            setLoginStatus(false);
        }
    }
}

void Client::incommingData() {
    auto array = _destination->readAll();

    if (_downloadPackage.hdr.isValid()) {
        _downloadPackage.data.append(array);

    } else {
        memcpy(&_downloadPackage.hdr,
               array.data(), sizeof(Header));
        _downloadPackage.data.append(array.mid(sizeof(Header)));
    }

    if (_downloadPackage.isValid()) {
        if (!receiveData(_downloadPackage.data, _downloadPackage.hdr)) {
            // ban
        }

        _downloadPackage.reset();
        return;
    }
}

void Client::handleDisconnected() {
    setOnlineStatus(false);
}

Client::Client(const QString &addrress, unsigned short port, QObject *ptr):
    QObject (ptr) {

    _destination = new QTcpSocket(this);
    connectToHost(addrress, port);

    connect(_destination, &QTcpSocket::readyRead,
            this, &Client::incommingData);

    connect(_destination, &QTcpSocket::disconnected,
            this, &Client::handleDisconnected);
}

bool Client::sendPackage(Package &pkg) {
    if (!pkg.isValid()) {
        return false;
    }

    if (!_destination->isValid()) {
        qCritical() << "destination server not valid!";
        return false;
    }

    if (!_destination->waitForConnected()) {
        qCritical() << "no connected to server! " << _destination->errorString();
        return false;
    }

    auto index = nextIndex();
    _requestsMap[index] = {{"expected", static_cast<const unsigned short>(pkg.hdr.command)}};
    pkg.hdr.sig = index;

    QByteArray bytes = pkg.toBytes();
    bool sendet = bytes.size() == _destination->write(bytes);

    return sendet;
}

unsigned char Client::nextIndex() {
    return static_cast<unsigned char>((currentIndex++) % 256);
}

bool Client::ping() {

    Package pcg;

    if (!pcg.create(Command::Ping, Type::Request)) {
        return false;
    };

    if (!sendPackage(pcg)) {
        return false;

    }

    return true;
}

///  Do not change the order of this function,
///  as this may lead to the loss of user accounts.
QByteArray Client::generateHash(const QByteArray& pass) const {
    auto passHash = QCryptographicHash::hash(pass, QCryptographicHash::Sha256);
    return QCryptographicHash::hash(SOLT + passHash, QCryptographicHash::Sha256);
}

bool Client::login(const QString &gmail, const QByteArray &pass, bool newUser) {
    if (!pass.size()) {
        return false;
    }

    if (!gmail.size()) {
        return false;
    }

    if (!isOnline()) {
        return false;
    }

    Package pcg;

    Login login;

    login.setHashPass(QRSAEncryption::encode(generateHash(pass), _rsaKey, QRSAEncryption::RSA_256));
    login.setGmail(gmail);
    login.setId(0);
    login.setRegisterNewUser(newUser);

    if (!login.isValid()) {
        return false;
    }

    if (!pcg.create(&login, Type::Request)) {
        return false;
    };

    if (!sendPackage(pcg)) {
        return false;

    }

    return true;
}

bool Client::registration(const QString &gmail,
                          const QByteArray &pass) {
    return login(gmail, pass, true);
}

void Client::loginOut() {
    _token = "";
    setLoginStatus(false);
}

void Client::dissconnectFromHost() {
    loginOut();
    _destination->disconnectFromHost();
}

void Client::connectToHost(const QString &address, unsigned short port) {
    _destination->connectToHost(_address = address, _port = port);
}

void Client::reconnectToHost() {
    _destination->connectToHost(_address, _port);
}

bool Client::updateData() {

    if (!isLogin()) {
        return false;
    }

    Package pcg;

    UpdatePlayerData rec;
    rec.setToken(_token);
    rec.setId(0);

    if (!rec.isValid()) {
        return false;
    }

    if (!pcg.create(&rec, Type::Request)) {
        return false;
    };

    if (!sendPackage(pcg)) {
        return false;
    }

    return true;
}

bool Client::savaData(const QList<int>& gameData) {
    if (!isLogin()) {
        return false;
    }

    Package pcg;

    GameData rec;
    rec.setToken(_token);
    rec.setTimeClick(gameData);
    rec.setId(0);

    if (!rec.isValid()) {
        return false;
    }

    if (!pcg.create(&rec, Type::Request)) {
        return false;
    };

    if (!sendPackage(pcg)) {
        return false;
    }

    return true;
}

bool Client::getItem(int id) {

    if (!isLogin()) {
        return false;
    }

    if (id <= 0) {
        return false;
    }

    Package pcg;

    GetItem rec;
    rec.setToken(_token);
    rec.setId(id);

    if (!rec.isValid()) {
        return false;
    }

    if (!pcg.create(&rec, Type::Request)) {
        return false;
    };

    if (!sendPackage(pcg)) {
        return false;
    }

    return true;
}

bool Client::getPlayer(int id){
    if (!isLogin()) {
        return false;
    }

    if (id <= 0) {
        return false;
    }

    Package pcg;

    UpdatePlayerData rec;
    rec.setToken(_token);
    rec.setId(id);

    if (!rec.isValid()) {
        return false;
    }

    if (!pcg.create(&rec, Type::Request)) {
        return false;
    };

    if (!sendPackage(pcg)) {
        return false;
    }

    return true;
}

const bool& Client::isOnline() const {
    return _online;
}

const bool& Client::isLogin() const {
    return _logined;
}

bool Client::setSubscribe(Command cmd, bool subscribe, int id) {
    if (!isLogin()) {
        return false;
    }

    Package pcg;

    WebSocket rec;
    rec.setId(0);
    rec.setToken(_token);
    rec.setCommand(cmd);
    rec.setObjectId(id);
    rec.setSubscribe(subscribe);

    if (!rec.isValid()) {
        return false;
    }

    if (!pcg.create(&rec, Type::Stream)) {
        return false;
    };

    if (!sendPackage(pcg)) {
        return false;
    }

    if (subscribe)
        _subscribe[static_cast<quint8>(cmd)] = false;
    else {
        _subscribe.remove(static_cast<quint8>(cmd));
    }

    return true;
}

QHash<quint8, bool> Client::getSubscribe() const {
    return _subscribe;
}


}
