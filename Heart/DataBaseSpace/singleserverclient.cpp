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

    connect(this, &SingleServerClient::requestError,
            this, &SingleServerClient::handleError);
}

ParserResult SingleServerClient::parsePackage(const Package &pkg,
                                              const AbstractNodeInfo *sender) {

    auto parentResult = DataBaseNode::parsePackage(pkg, sender);
    if (parentResult != QH::ParserResult::NotProcessed) {
        return parentResult;
    }

    if (H_16<QH::PKG::UserMember>() == pkg.hdr.command) {
        QH::PKG::UserMember obj(pkg);

        if (!(obj.isValid() && obj.token().isValid())) {
            return ParserResult::Error;
        }

        setMember(obj);
        setStatus(ClientStatus::Logined);
        incomingData(&obj, sender);

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
        QuasarAppUtils::Params::log("You try make login on alredy logined client."
                                    " Please run logout method befor login.",
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
        QuasarAppUtils::Params::log("You try make connect on the online client."
                                    " This client alredy connected to server.",
                                    QuasarAppUtils::Error);
        return false;
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
        QTimer::singleShot(WAIT_RESPOCE_TIME, [this, oldStatus]() {
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
    QH::PKG::AuthRequest request;
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

void SingleServerClient::setMember(const PKG::UserMember &member) {
    _member = member;
}

ErrorCodes::Code SingleServerClient::getLastError() const {
    return _lastError;
}

QString SingleServerClient::getLastErrorString() const {
    return ErrorCodes::DBErrorCodesHelper::toString(_lastError);
}

void SingleServerClient::setLastError(const ErrorCodes::Code &lastError) {
    _lastError = lastError;
}
}
