/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "singleserverclient.h"
#include "quasarapp.h"

namespace QH {

SingleServerClient::SingleServerClient() {

}

ParserResult SingleServerClient::parsePackage(const Package &pkg, const AbstractNodeInfo *sender) {

}

ClientStatus SingleServerClient::getStatus() const {
    return _status;
}

bool SingleServerClient::login(const QString &userId, const QString &rawPassword) {

}

bool SingleServerClient::signup(const QString &userId, const QString &rawPassword) {

}

void SingleServerClient::setStatus(const ClientStatus &status) {
    if (status == _status)
        return;

    _status = status;
    emit statusChanged(_status);
}

void SingleServerClient::handleError(unsigned char, const QString &error) {
    QuasarAppUtils::Params::log(error, QuasarAppUtils::Error);
}

bool SingleServerClient::p_login(const QString &userId, const QByteArray &hashPassword) {

}

bool SingleServerClient::p_signIn(const QString &userId, const QByteArray &hashPassword) {

}
}
