#include "server.h"
#include <QLocalSocket>
#include <quasarapp.h>
#include <cstring>
#include <QFile>
#include <QDataStream>
#include <QVariantMap>

#include "serverutils.h"
#include "serverprotocol.h"
#include "server.h"


namespace ServerProtocol {

void Server::parsePackage(const Package& pkg) {
    if (!pkg.isValid()) {
        return;
    }


    switch (pkg.hdr.command) {
    case Ping: {

        if (pkg.hdr.type != Request) {
            return;
        }

        Package resp;
        resp.hdr.command = Ping;
        resp.hdr.type = Responke;

        QVariantMap data;
        data["res"] = "Pong";
        resp.fromMap(data);

        if (!sendPackage(resp)) {
            QuasarAppUtils::Params::verboseLog("!responce not sendet!");
        }

        break;
    }

    default: {
        QVariantMap res;
        QDataStream stream(pkg.data);
        stream >> res;
        res["command"] = pkg.hdr.command;
        emit incomingRequest(res);
    };
    }
}

bool Server::sendPackage(Package &pkg) {
    if (!pkg.isValid()) {
        return false;
    }

    if (!_client->isValid()) {
        qCritical() << "destination server not valid!";
        return false;
    }

    if (!_client->waitForConnected()) {
        qCritical() << "no connected to server! " << _client->errorString();
        return false;
    }

    auto bytes = pkg.toBytes();
    bool sendet = bytes.size() == _client->write(bytes);

    return sendet;
}


void Server::avelableBytes() {
    auto array = _client->readAll();

    if (_downloadPackage.hdr.isValid()) {
        _downloadPackage.data.append(array);

    } else {
        memcpy(&_downloadPackage.hdr,
               array.data(), sizeof(Header));
        _downloadPackage.data.append(array.mid(sizeof(Header)));
    }

    if (_downloadPackage.isValid()) {
        parsePackage(_downloadPackage);
        _downloadPackage.reset();
        return;
    }
}

void Server::incomingConnection(quintptr socketDescriptor) {
    _client->setSocketDescriptor(static_cast<qintptr>(socketDescriptor));

    if (!_client->isValid()) {
        QuasarAppUtils::Params::verboseLog("incomingConnection not valid!");
        return;
    }

    if (!_client->isOpen() && !_client->open()) {
        QuasarAppUtils::Params::verboseLog("incomingConnection not opened!");
        return;
    }
}

Server::Server(QObject *ptr):
    QLocalServer (ptr)
{
    _client = new QLocalSocket(this);

    connect(_client, &QLocalSocket::readyRead,
            this, &Server::avelableBytes);
}

Server::~Server() {
    close();
}

bool Server::run(const QString &name, bool force) {

    if (force) {
        QFile::remove("/tmp/" + name);
    }

    if (!listen(name)) {
        QuasarAppUtils::Params::verboseLog("listing fail " + this->errorString());
        return false;
    }

    return true;
}

bool Server::sendResponce(QVariantMap res, Command command) {

    Package pck;

    pck.hdr.type = ServerProtocol::Responke;
    pck.hdr.command = static_cast<unsigned char>(command);

    if (res.isEmpty()) {
        res["Error"] = "command not supported!";
    }

    pck.fromMap(res);
    pck.hdr.size = static_cast<unsigned short>(pck.data.size());

    return sendPackage(pck);
}
}
