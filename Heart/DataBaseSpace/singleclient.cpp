/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "singleclient.h"
#include "quasarapp.h"
#include "authrequest.h"
#include "dberrorcodes.h"

#include <websocket.h>

namespace QH {

SingleClient::SingleClient() {

    qRegisterMetaType<QH::ClientStatus>();

    registerPackageType<PKG::AuthRequest>();

    connect(this, &SingleClient::requestError,
            this, &SingleClient::handleError);
}

ParserResult SingleClient::parsePackage(const QSharedPointer<PKG::AbstractData> &pkg,
                                        const Header& pkgHeader,
                                        const AbstractNodeInfo* sender) {

    auto parentResult = DataBaseNode::parsePackage(pkg, pkgHeader, sender);
    if (parentResult != QH::ParserResult::NotProcessed) {
        return parentResult;
    }

    if (H_16<QH::PKG::UserMember>() == pkg->cmd()) {
        QH::PKG::UserMember *obj = static_cast<QH::PKG::UserMember*>(pkg.data());

        if (!(obj->isValid() && obj->getSignToken().isValid())) {
            return ParserResult::Error;
        }

        setMember(*obj);
        setStatus(ClientStatus::Logined);

        emit currentUserChanged();

        return QH::ParserResult::Processed;

    }

    handleRestRequest(pkg, pkgHeader);

    return QH::ParserResult::NotProcessed;
}

ClientStatus SingleClient::getStatus() const {
    return _status;
}

bool SingleClient::login(const QString &userId, const QString &rawPassword) {
    if (getStatus() < ClientStatus::Connected) {
        QuasarAppUtils::Params::log("You try make login on the offline client."
                                    " Please wait of ClientStatus::Connected status.",
                                    QuasarAppUtils::Error);
        return false;
    }

    if (!p_login(userId, hashgenerator(rawPassword.toLatin1()))) {
        return false;
    };

    if (getStatus() < ClientStatus::Loginning) {
        setStatus(ClientStatus::Loginning);
    }

    return true;
}

bool SingleClient::login(const PKG::UserMember &memberData) {

    if (memberData.getSignToken() == getMember().getSignToken()) {
        if (isLogined()) {
            return true;
        }

        return login();
    }

    setMember(memberData);
    return login();
}



bool SingleClient::logout() {
    if (getStatus() < ClientStatus::Logined) {
        QuasarAppUtils::Params::log("You try logout on the not Loggined client."
                                    " Please wait of ClientStatus::Logined status.",
                                    QuasarAppUtils::Error);
        return false;
    }


    QH::PKG::AuthRequest request;
    request.copyFrom(&getMember());
    request.setRequest(PKG::UserRequestType::LogOut);

    if (!sendData(&request, serverAddress())) {
        return false;
    };

    resetUser();

    return true;
}

bool SingleClient::signup(const QString &userId, const QString &rawPassword) {
    if (getStatus() < ClientStatus::Connected) {
        QuasarAppUtils::Params::log("You try make signup on the offline client."
                                    " Please wait of ClientStatus::Connected status.",
                                    QuasarAppUtils::Error);
        return false;
    }

    if (!p_signup(userId, hashgenerator(rawPassword.toLatin1()))) {
        return false;
    };

    if (getStatus() < ClientStatus::Loginning) {
        setStatus(ClientStatus::Loginning);
    }

    return true;
}

bool SingleClient::removeUser() {
    if (getStatus() < ClientStatus::Logined) {
        QuasarAppUtils::Params::log("You try make remove on the not Loggined client."
                                    " Please wait of ClientStatus::Logined status.",
                                    QuasarAppUtils::Error);
        return false;
    }


    QH::PKG::AuthRequest request;
    request.copyFrom(&getMember());
    request.setRequest(PKG::UserRequestType::Remove);
    if (!sendData(&request, serverAddress())) {
        return false;
    };

    return true;
}

bool SingleClient::connectToServer() {
    if (getStatus() >= ClientStatus::Connected) {
        QuasarAppUtils::Params::log(" This client alredy connected to server.",
                                    QuasarAppUtils::Warning);
        return true;
    }

    if (getStatus() < ClientStatus::Connecting) {
        setStatus(ClientStatus::Connecting);
    }

    addNode(serverAddress());

    return true;
}

void SingleClient::disconnectFromServer() {
    if (getStatus() == ClientStatus::Dissconnected) {
        return;
    }

    removeNode(serverAddress());
    setStatus(ClientStatus::Dissconnected);
}

void SingleClient::setStatus(const ClientStatus &status) {
    if (status == _status)
        return;

    auto oldStatus = _status;

    if (status == ClientStatus::Connecting || status == ClientStatus::Loginning) {
        QTimer::singleShot(WAIT_RESPOCE_TIME, this, [this, oldStatus]() {
            if (_status == ClientStatus::Connecting || _status == ClientStatus::Loginning) {
                setStatus(oldStatus);
                emit requestError(static_cast<unsigned char >(ErrorCodes::TimeOutError),
                                  ErrorCodes::DBErrorCodesHelper::toString(ErrorCodes::TimeOutError));
            }
        });
    }

    _status = status;
    emit statusChanged(_status);
}

void SingleClient::handleError(unsigned char code, QString error) {
    QuasarAppUtils::Params::log(error, QuasarAppUtils::Error);

    if (code != ErrorCodes::NoError && getStatus() == ClientStatus::Loginning) {
        setStatus(ClientStatus::Connected);
    }

    _lastError = code;
}

bool SingleClient::p_login(const QString &userId, const QByteArray &hashPassword) {
    auto userMember = getMember();
    if ((userId.isEmpty() || hashPassword.isEmpty()) && !userMember.isValid()) {
        return false;
    }

    QH::PKG::AuthRequest request;
    if (userId.isEmpty())
        request.setId(userMember.getId());
    else
        request.setId(userId);

    request.setRequest(QH::PKG::UserRequestType::LogIn);

    if (hashPassword.isEmpty()) {
        const auto &member = getMember();
        if (!member.getSignToken().isValid()) {
            return false;
        }

        request.setSignToken(member.getSignToken());
    } else {
        request.setAuthenticationData(hashPassword);
    }

    return sendData(&request, serverAddress());
}

bool SingleClient::p_signup(const QString &userId, const QByteArray &hashPassword) {
    QH::PKG::AuthRequest request;
    request.setId(userId);
    request.setAuthenticationData(hashPassword);
    request.setRequest(QH::PKG::UserRequestType::SignUp);

    return sendData(&request, serverAddress());
}

void SingleClient::handleRestRequest(const QSharedPointer<PKG::AbstractData> &pkg,
                                     const Header& pkgHeader) {
    QMutexLocker lock(&_handlerCacheMutex);

    if (_handlersCache.contains(pkgHeader.triggerHash)) {
        auto action = _handlersCache.take(pkgHeader.triggerHash);
        action(pkg);
    }
}

const PKG::UserMember &SingleClient::getMember() const {
    return _member;
}

HostAddress SingleClient::serverAddress() const {
    return HostAddress{"localhost", DEFAULT_PORT};
}

void SingleClient::nodeConfirmend(AbstractNodeInfo *node) {
    Q_UNUSED(node)
}

void QH::SingleClient::nodeConnected(AbstractNodeInfo *node) {
    Q_UNUSED(node)
    setStatus(ClientStatus::Connected);
}

void QH::SingleClient::nodeDisconnected(AbstractNodeInfo *node) {
    Q_UNUSED(node)
    setStatus(ClientStatus::Dissconnected);
}

unsigned int SingleClient::sendData(PKG::AbstractData *resp,
                                    const HostAddress &address,
                                    const Header *req) {

    if (!signPackageWithToken(resp)) {
        return 0;
    }

    return DataBaseNode::sendData(resp, address, req);
}

unsigned int SingleClient::sendData(PKG::AbstractData *resp,
                                    const QVariant &nodeId,
                                    const Header *req) {

    if (!signPackageWithToken(resp)) {
        return 0;
    }

    return DataBaseNode::sendData(resp, nodeId, req);
}

void SingleClient::resetUser() {
    setMember({});
    if (getStatus() > ClientStatus::Connected)
        setStatus(ClientStatus::Connected);
}

void SingleClient::setMember(const PKG::UserMember &member) {
    QMutexLocker lock (&_handlerMemberMutex);
    _member = member;
}

ErrorCodes::Code SingleClient::getLastError() const {
    return _lastError;
}

QString SingleClient::getLastErrorString() const {
    return ErrorCodes::DBErrorCodesHelper::toString(_lastError);
}

bool SingleClient::isConnected() const {
    return getStatus() >= ClientStatus::Connected;
}

bool SingleClient::isLogined() const {
    return getStatus() >= ClientStatus::Logined;
}

bool SingleClient::subscribe(unsigned int id) {
    if (getStatus() < ClientStatus::Logined) {
        return false;
    }

    PKG::WebSocket request;
    request.setSubscribeId(id);
    request.setRequestCommnad(PKG::WebSocketRequest::Subscribe);

    return sendData(&request, serverAddress());
}

bool SingleClient::unsubscribe(unsigned int id) {
    if (getStatus() < ClientStatus::Logined) {
        return false;
    }

    PKG::WebSocket request;
    request.setSubscribeId(id);
    request.setRequestCommnad(PKG::WebSocketRequest::Unsubscribe);

    return sendData(&request, serverAddress());

}


bool SingleClient::signPackageWithToken(PKG::AbstractData *pkg) const {
    auto tokenObject = dynamic_cast<IToken*>(pkg);
    if (!tokenObject) {
        return true;
    }

    auto token = getMember().getSignToken();

    if (!token.isValid())
        return false;

    tokenObject->setSignToken(token);

    return true;
}

bool SingleClient::restRequest(PKG::AbstractData *req,
                               const HandlerMethod &handler) {


    if (!signPackageWithToken(req)) {
        QuasarAppUtils::Params::log("Fail to sign package with token", QuasarAppUtils::Error);
        return false;
    }

    unsigned int pkgHash = sendData(req, serverAddress());

    if (!pkgHash)
        return false;

    QMutexLocker lock(&_handlerCacheMutex);
    _handlersCache[pkgHash] = handler;

    QTimer::singleShot(WAIT_RESPOCE_TIME, this, [this, pkgHash]() {
        QMutexLocker lock(&_handlerCacheMutex);
        _handlersCache.remove(pkgHash);
    });

    return true;
}

void SingleClient::setLastError(const ErrorCodes::Code &lastError) {
    _lastError = lastError;
}

}
