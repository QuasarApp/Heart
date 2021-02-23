/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "singleserverclient.h"
#include "quasarapp.h"
#include "authrequest.h"
#include "dberrorcodes.h"

namespace QH {

SingleServerClient::SingleServerClient() {

    qRegisterMetaType<QH::ClientStatus>();

    registerPackageType<PKG::UserMember>();

    connect(this, &SingleServerClient::requestError,
            this, &SingleServerClient::handleError);
}

ParserResult SingleServerClient::parsePackage(const QSharedPointer<PKG::AbstractData> &pkg,
                                              const Header& pkgHeader,
                                              const AbstractNodeInfo* sender) {

    auto parentResult = DataBaseNode::parsePackage(pkg, pkgHeader, sender);
    if (parentResult != QH::ParserResult::NotProcessed) {
        return parentResult;
    }

    if (H_16<QH::PKG::UserMember>() == pkg->cmd()) {
        QH::PKG::UserMember *obj = static_cast<QH::PKG::UserMember*>(pkg.data());

        if (!(obj->isValid() && obj->token().isValid())) {
            return ParserResult::Error;
        }

        setMember(*obj);
        setStatus(ClientStatus::Logined);
        incomingData(obj, sender);

        return QH::ParserResult::Processed;

    }

    return QH::ParserResult::NotProcessed;
}

ClientStatus SingleServerClient::getStatus() const {
    return _status;
}

bool SingleServerClient::login(const QString &userId, const QString &rawPassword) {
    if (getStatus() < ClientStatus::Connected) {
        QuasarAppUtils::Params::log("You try make login on the offline client."
                                    " Please wait of ClientStatus::Connected status.",
                                    QuasarAppUtils::Error);
        return false;
    }

    if (getStatus() == ClientStatus::Logined) {
        QuasarAppUtils::Params::log("This cliet is alredy logined.",
                                    QuasarAppUtils::Warning);
        return true;
    }

    if (!p_login(userId, hashgenerator(rawPassword.toLatin1()))) {
        return false;
    };

    if (getStatus() < ClientStatus::Loginning) {
        setStatus(ClientStatus::Loginning);
    }

    return true;
}

bool SingleServerClient::login(const PKG::UserMember &memberData) {

    if (memberData.token() == getMember().token()) {
        if (isLogined()) {
            return true;
        }

        return login();
    }

    setMember(memberData);
    return login();
}

bool SingleServerClient::logout() {
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

    setMember({});
    if (getStatus() > ClientStatus::Connected)
        setStatus(ClientStatus::Connected);

    return true;
}

bool SingleServerClient::signup(const QString &userId, const QString &rawPassword) {
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

bool SingleServerClient::removeUser() {
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

bool SingleServerClient::connectToServer() {
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

void SingleServerClient::disconnectFromServer() {
    if (getStatus() == ClientStatus::Dissconnected) {
        return;
    }

    removeNode(serverAddress());
    setStatus(ClientStatus::Dissconnected);
}

void SingleServerClient::setStatus(const ClientStatus &status) {
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

void SingleServerClient::handleError(unsigned char code, QString error) {
    QuasarAppUtils::Params::log(error, QuasarAppUtils::Error);

    if (code != ErrorCodes::NoError && getStatus() == ClientStatus::Loginning) {
        setStatus(ClientStatus::Connected);
    }

    _lastError = code;
}

bool SingleServerClient::p_login(const QString &userId, const QByteArray &hashPassword) {
    auto userMember = getMember();
    if ((userId.isEmpty() || hashPassword.isEmpty()) && !userMember.isValid()) {
        return false;
    }

    QH::PKG::AuthRequest request;
    if (userId.isEmpty())
        request.setName(userMember.name());
    else
        request.setName(userId);

    request.setRequest(QH::PKG::UserRequestType::LogIn);

    if (hashPassword.isEmpty()) {
        const auto &member = getMember();
        if (!member.token().isValid()) {
            return false;
        }

        request.setToken(member.token());
    } else {
        request.setAuthenticationData(hashPassword);
    }

    return sendData(&request, serverAddress());
}

bool SingleServerClient::p_signup(const QString &userId, const QByteArray &hashPassword) {
    QH::PKG::AuthRequest request;
    request.setName(userId);
    request.setAuthenticationData(hashPassword);
    request.setRequest(QH::PKG::UserRequestType::SignUp);

    return sendData(&request, serverAddress());
}

const PKG::UserMember &SingleServerClient::getMember() const {
    return _member;
}

HostAddress SingleServerClient::serverAddress() const {
    return HostAddress{"localhost", DEFAULT_PORT};
}

void SingleServerClient::nodeConfirmend(AbstractNodeInfo *node) {
    Q_UNUSED(node)
}

void QH::SingleServerClient::nodeConnected(AbstractNodeInfo *node) {
    Q_UNUSED(node)
    setStatus(ClientStatus::Connected);
}

void QH::SingleServerClient::nodeDisconnected(AbstractNodeInfo *node) {
    Q_UNUSED(node)
    setStatus(ClientStatus::Dissconnected);
}

bool SingleServerClient::sendData(const PKG::AbstractData *resp, const HostAddress &address, const Header *req) {

    if (!checkToken(resp)) {

        QuasarAppUtils::Params::log("For The SingleServerClient classes you must be add support of the Token validation."
                                    " All package classes must be inherited of the IToken interface",
                                    QuasarAppUtils::Error);

        return false;
    }

    return DataBaseNode::sendData(resp, address, req);
}

void SingleServerClient::setMember(const PKG::UserMember &member) {
    _member = member;
}

ErrorCodes::Code SingleServerClient::getLastError() const {
    return _lastError;
}

QString SingleServerClient::getLastErrorString() const {
    return ErrorCodes::DBErrorCodesHelper::toString(_lastError);
}

bool SingleServerClient::isConnected() const {
    return getStatus() >= ClientStatus::Connected;
}

bool SingleServerClient::isLogined() const {
    return getStatus() >= ClientStatus::Logined;
}

void SingleServerClient::setLastError(const ErrorCodes::Code &lastError) {
    _lastError = lastError;
}

}
