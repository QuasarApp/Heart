/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "client.h"

#include <QHostInfo>
#include <userdata.h>
#include <userrequest.h>
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
    auto user = SP<UserRequest>::create();
    user->setMail(userMail);
    user->setPassSHA256(hashgenerator(rawPath));
    user->setRequestCmd(static_cast<quint8>(UserRequestCmd::Login));
    _user->copyFrom(user.data());

    return sendData(user, _address);
}

bool Client::login() {
    if (_user->mail().size()) {

        if (!(_user->token().isValid() || _user->passSHA256().size())) {
            return false;
        }

        auto request = SP<UserRequest>::create();
        request->copyFrom(_user.data());
        request->setRequestCmd(static_cast<quint8>(UserRequestCmd::Login));

        return sendData(request, _address);

    }


    return false;
}

bool Client::logout() {
    _user->setToken(AccessToken());

    if (status() == Logined)
        setStatus(Online);
    return !_user->token().isValid();
}

bool Client::removeProfile() {
    if (_user->mail().size()) {

        if (!(_user->token().isValid() || _user->passSHA256().size())) {
            return false;
        }

        auto request = SP<UserRequest>::create();
        request->copyFrom(_user.data());
        request->setRequestCmd(static_cast<quint8>(UserRequestCmd::Delete));

        return sendData(request, _address);

    }


    return false;
}

bool Client::syncUserData() {
    if (_status == Status::Offline) {
        return false;
    }

    if (!_user->isValid()) {
        return false;
    }

    SP<UserRequest> request;
    *request.dynamicCast<UserData>() = *_user;
    request->setRequestCmd(static_cast<unsigned char>(UserRequestCmd::Save));

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

    if (_user->mail() == userData->mail()) {
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

QHostAddress Client::address() const {
    return _address;
}

QSharedPointer<UserData> Client::user() const {
    return _user;
}

bool Client::setUser(const UserData *user) {
    if (!_user->copyFrom(user)) {
        return false;
    }

    return true;
}

void Client::setStatus(Client::Status status) {
    if (status != _status) {
        _status = status;
        emit statusChanged(_status);
    }
}

}
