/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "client.h"

#include <QHostInfo>
#include <userdata.h>
#include <userdatarequest.h>
namespace NP {

Client::Client() {

    connect(this, &BaseNode::incomingData,
            this, &Client::handleIncomingData,
            Qt::DirectConnection);

    _user = SP<UserData>::create();
}

Client::Client(const QHostAddress &address, unsigned short port):
    Client() {

    setHost(address, port);

}

Client::Client(const QString &domain, unsigned short port):
    Client() {
    setHost(domain, port);
}

void Client::connectClient() {
    if (!_address.isNull())
        connectToHost(_address, _port);
    else
        connectToHost(_domain, _port);
}

void Client::setHost(const QString &address, unsigned short port) {
    _domain = address;
    _port = port;
}

void Client::setHost(const QHostAddress &address, unsigned short port) {
    _address = address;
    _port = port;
}

bool Client::login(const QString &userMail, const QByteArray &rawPath) {
    auto user = SP<UserDataRequest>::create();
    user->setMail(userMail);
    user->setPassSHA256(hashgenerator(rawPath));
    user->setRequestCmd(static_cast<quint8>(UserDataRequestCmd::Login));


    return _user->copyFrom(user.data()) && sendData(user, _address);
}

bool Client::logout() {
    _user->setToken(AccessToken());

    if (status() == Logined)
        setStatus(Online);
    return !_user->token().isValid();
}

bool Client::syncUserData() {
    if (_status == Status::Offline) {
        return false;
    }

    if (!_user->isValid()) {
        return false;
    }

    SP<UserDataRequest> request;
    *request.dynamicCast<UserData>() = *_user;
    request->setRequestCmd(static_cast<unsigned char>(UserDataRequestCmd::Save));

    return sendData(request, _address);
}

int Client::status() const {
    return _status;
}

QString Client::lastMessage() const {
    return _lastMessage;
}

bool Client::registerSocket(QAbstractSocket *socket, const QHostAddress *clientAddress) {

    if (!BaseNode::registerSocket(socket, clientAddress)) {
        return false;
    }

    auto info = getInfoPtr(*clientAddress);

    if (info.isNull()) {
        return false;
    }

    auto strongInfo = info.toStrongRef();

    connect(strongInfo->sct(), &QAbstractSocket::stateChanged,
            this, &Client::socketStateChanged);

    return true;
}

void Client::connectToHost(const QHostAddress &ip, unsigned short port, SslMode mode) {
    RatingUserNode::connectToHost(ip, port, mode);
    setHost(ip, port);
}

void Client::connectToHost(const QString &domain, unsigned short port, SslMode mode) {
    RatingUserNode::connectToHost(domain, port, mode);
}

void Client::handleIncomingData(SP<AbstractData> obj,
                                const QHostAddress&) {

    auto userData = obj.dynamicCast<UserData>();

    if (userData.isNull()) {
        return;
    }

    if (_user->mail() == userData->mail()
            && _user->passSHA256() == userData->passSHA256()) {
        _user->copyFrom(userData.data());
        setStatus(Status::Logined);
    }

}

void Client::setLastMessage(QString lastMessage) {
    if (_lastMessage == lastMessage)
        return;

    _lastMessage = lastMessage;
    emit lastMessageChanged(_lastMessage);
}

void Client::socketStateChanged(QAbstractSocket::SocketState state) {
    if (state < QAbstractSocket::ConnectedState) {
        setStatus(Status::Offline);

    } else if (_status != Logined) {
        setStatus(Status::Online);
    }
}

QSharedPointer<UserData> Client::user() const {
    return _user;
}

void Client::setStatus(Client::Status status) {
    if (status != _status) {
        _status = status;
        emit statusChanged(_status);
    }
}

}
