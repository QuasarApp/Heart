#include "client.h"
#include "serverprotocol.h"

#include <QLocalSocket>
#include <quasarapp.h>
#include <cstring>
#include <QProcess>
#include <QDateTime>
#include <QCoreApplication>

namespace ServerProtocol {

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
        emit sigIncommingData(_downloadPackage.parse());
        _downloadPackage.reset();
        received = true;
        return;
    }
}

Client::Client(const QString& server, QObject *ptr):
    QObject (ptr) {

    _destination = new QLocalSocket(this);

    _destination->connectToServer(server);

    connect(_destination, &QLocalSocket::readyRead,
            this, &Client::incommingData);

}

bool Client::sendPackage(const Package &pkg) {
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

    auto bytes = pkg.toBytes();

    return bytes.size() == _destination->write(bytes);
}

bool Client::ping() {
    ServerProtocol::Package pkg;
    pkg.hdr.command = ServerProtocol::Ping;
    pkg.hdr.type = ServerProtocol::Request;

    return sendPackage(pkg);
}

bool Client::getState() {

    ServerProtocol::Package pkg;
    pkg.hdr.command = ServerProtocol::State;
    pkg.hdr.type = ServerProtocol::Request;

    return sendPackage(pkg);
}

bool Client::ban(const QHostAddress &address) {

    ServerProtocol::Package pkg;
    pkg.hdr.command = ServerProtocol::Ban;
    pkg.hdr.type = ServerProtocol::Request;

    if (address.isNull())
        return false;

    QVariantMap map;
    map["address"] = address.toIPv4Address();

    pkg.fromMap(map);

    return sendPackage(pkg);
}

bool Client::unBan(const QHostAddress &address) {

    if (address.isNull())
        return false;

    ServerProtocol::Package pkg;
    pkg.hdr.command = ServerProtocol::Unban;
    pkg.hdr.type = ServerProtocol::Request;

    QVariantMap map;
    map["address"] = address.toIPv4Address();

    pkg.fromMap(map);

    return sendPackage(pkg);
}

bool Client::restart(const QString &address, unsigned short port) {

    QHostAddress test(address);

    if (test.isNull() || port == 0) {
        return false;
    }

    ServerProtocol::Package pkg;
    pkg.hdr.command = ServerProtocol::Restart;
    pkg.hdr.type = ServerProtocol::Request;

    QVariantMap map;
    map["address"] = address;
    map["port"] = port;
    pkg.fromMap(map);

    return sendPackage(pkg);
}

bool Client::start(const QString &address, unsigned short port) {
    QHostAddress test(address);

    QStringList params = {"daemon"};

    if (!test.isNull()) {
        params.push_back("-address");
        params.push_back(address);

    }

    if (port > 0) {
        params.push_back("-port");
        params.push_back(QString::number(port));
    }

    QProcess p;
    QProcessEnvironment env;

    p.setProcessEnvironment(env);
    p.start("snake-d", params);

    if (p.waitForFinished(1000) && p.exitCode() == 0) {
        emit sigIncommingData({{"Res", "server started"}});
    } else {
        emit sigIncommingData({{"Res", "server started fail " + p.readAll()}});
    }

    return true;
}

bool Client::stop() {

    ServerProtocol::Package pkg;
    pkg.hdr.command = ServerProtocol::Stop;
    pkg.hdr.type = ServerProtocol::Request;

    QVariantMap map;
    pkg.fromMap(map);

    return sendPackage(pkg);
}

bool Client::wait(bool & forWait, int msec) {
    auto curmsec = QDateTime::currentMSecsSinceEpoch() + msec;
    while (curmsec > QDateTime::currentMSecsSinceEpoch() && !forWait) {
        QCoreApplication::processEvents();
    }
    QCoreApplication::processEvents();
    return forWait;
}

bool Client::wait(int msec)
{
    received = false;
    return wait(received, msec);
}
}
