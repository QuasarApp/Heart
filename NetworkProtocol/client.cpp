﻿#include "client.h"

#include <userdata.h>
#include <userdatarequest.h>
namespace NP {

Client::Client(const QHostAddress &address, unsigned short port) {
    setHost(address, port);

    connect(this, &BaseNode::incomingData,
            this, &Client::handleIncomingData,
            Qt::DirectConnection);

    _user = SP<UserData>::create();
}

Client::Client(const QString &address, unsigned short port):
    Client(QHostAddress(address), port){
}

bool Client::connectClient() {
    connectToHost(_address, _port);

    auto info = getInfoPtr(_address).toStrongRef();

    if (info.isNull()) {
        return false;
    }

    connect(info->sct(), &QAbstractSocket::stateChanged,
            this, &Client::socketStateChanged);

    return true;

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

void Client::handleIncomingData(SP<AbstractData> obj,
                                const QHostAddress&) {

    auto userData = obj.dynamicCast<UserData>();

    if (userData.isNull()) {
        return;
    }

    if (_user->mail() == userData->mail()
            && _user->passSHA256() == userData->passSHA256()) {
        _user = userData;
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

void Client::setStatus(Client::Status status) {
    if (status != _status) {
        _status = status;
        emit statusChanged(_status);
    }
}

}
