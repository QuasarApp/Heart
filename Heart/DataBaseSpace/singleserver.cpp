/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "itoken.h"
#include "singleserver.h"

#include <authrequest.h>
#include <sqldbcache.h>
#include <basenodeinfo.h>
#include <badrequest.h>
#include <getmaxintegerid.h>

namespace QH {

SingleServer::SingleServer()
{

    registerPackageType<QH::PKG::AuthRequest>();
}

ErrorCodes::Code SingleServer::registerNewUser(PKG::UserMember user,
                                                 const AbstractNodeInfo *info) {
    if (!db()) {
        return ErrorCodes::InternalError;
    }

    auto localObject = db()->getObject(user);

    if (localObject) {
        return ErrorCodes::UserExits;
    }

    auto rawPassword = user.authenticationData();
    user.setAuthenticationData(hashgenerator(rawPassword));

    if (!db()->insertObject(QSharedPointer<decltype(user)>::create(user))) {
        return ErrorCodes::InternalError;
    };

    user.setAuthenticationData(rawPassword);

    // get id of the user
    auto userID = db()->getObject(PKG::GetMaxIntegerId(user.tableName(), "id"))->value();
    user.setId(userID);
    addUpdatePermission(userID, user.dbAddress(), Permission::Write);

    return loginUser(user, info);
}

// To-Do fix me bug  #12 https://github.com/QuasarApp/Heart/issues/12
ErrorCodes::Code SingleServer::loginUser(const PKG::UserMember &user,
                                         const AbstractNodeInfo *info) {

    if (!db()) {
        return ErrorCodes::InternalError;
    }

    auto localObject = db()->getObject(user);

    if (!localObject) {
        return ErrorCodes::UserNotExits;
    }

    auto nodeinfo = dynamic_cast<const BaseNodeInfo*>(info);
    if (!nodeinfo)
        return ErrorCodes::InternalError;

    AccessToken token((nodeinfo->token().toBytes()));

    if (token.isValid()) {
        return ErrorCodes::UserAlreadyLogged;
    }

    token = localObject->token();
    if (!token.isValid()) {
        token = generateToken(AccessToken::Year);
    }

    if (token != user.token()) {
        if (localObject->authenticationData() != hashgenerator(user.authenticationData())) {
            return ErrorCodes::UserInvalidPasswoed;
        }
    }

    auto editableNodeInfo = static_cast<BaseNodeInfo*>(getInfoPtr(info->networkAddress()));
    if (!editableNodeInfo)
        return ErrorCodes::InternalError;

    editableNodeInfo->setToken(token);
    editableNodeInfo->setId(localObject->getId());

    localObject->setToken(token);
    if (!db()->updateObject(localObject)) {
        return ErrorCodes::InternalError;
    }

    localObject->setAuthenticationData("");
    if (!sendData(localObject.data(), info->networkAddress())) {
        return ErrorCodes::InternalError;
    };

    return ErrorCodes::NoError;
}

ErrorCodes::Code SingleServer::logOutUser(const PKG::UserMember &user,
                                          const AbstractNodeInfo *info) {

    if (!db()) {
        return ErrorCodes::InternalError;
    }

    auto localObject = db()->getObject(user);

    if (!localObject) {
        return ErrorCodes::UserNotExits;
    }

    auto nodeinfo = dynamic_cast<const BaseNodeInfo*>(info);
    if (!nodeinfo)
        return ErrorCodes::InternalError;

    AccessToken token((nodeinfo->token().toBytes()));
    if (!token.isValid()) {
        return ErrorCodes::UserNotLogged;
    }

    if (user.token() != token) {
        return ErrorCodes::UserNotLogged;
    }

    auto mutableNodeInfo = dynamic_cast<BaseNodeInfo*>(getInfoPtr(info->networkAddress()));
    if (!mutableNodeInfo)
        return ErrorCodes::InternalError;

    mutableNodeInfo->setToken(AccessToken{});
    mutableNodeInfo->setId({});

    return ErrorCodes::NoError;

}

bool SingleServer::signValidation(const PKG::AbstractData *data, const AbstractNodeInfo *sender) const {
    auto iToken = dynamic_cast<const IToken*>(data);
    auto senderInfo = dynamic_cast<const BaseNodeInfo*>(sender);

    if (!iToken || !senderInfo)
        return false;

    return iToken->getSignToken() == senderInfo->token();

}

AccessToken SingleServer::generateToken(int length) {
    return AccessToken(length);
}

ParserResult SingleServer::parsePackage(PKG::AbstractData *pkg,
                                        const Header &pkgHeader,
                                        const AbstractNodeInfo *sender) {
    auto parentResult = DataBaseNode::parsePackage(pkg, pkgHeader, sender);
    if (parentResult != QH::ParserResult::NotProcessed) {
        return parentResult;
    }

    if (!signValidation(pkg, sender)) {

        prepareAndSendBadRequest(sender->networkAddress(), pkgHeader,
                                 ErrorCodes::OperatioForbiden, REQUEST_ERROR);

        return ParserResult::Error;
    };

    if (H_16<QH::PKG::AuthRequest>() == pkg->cmd()) {
            auto obj = QSharedPointer<QH::PKG::AuthRequest>(static_cast<PKG::AuthRequest*>(pkg));

            if (!obj->isValid()) {
                prepareAndSendBadRequest(sender->networkAddress(), pkgHeader,
                                         ErrorCodes::InvalidRequest, REQUEST_ERROR);
                return ParserResult::Error;
            }

            if (!workWithUserRequest(obj, pkgHeader, sender)) {
                return QH::ParserResult::Error;
            }

            return QH::ParserResult::Processed;

        }

    return QH::ParserResult::NotProcessed;

}

QByteArray SingleServer::hashgenerator(const QByteArray &data) const {
    return DataBaseNode::hashgenerator(data + "singelserversoult");
}

bool SingleServer::workWithUserRequest(const QSharedPointer<PKG::UserMember>& obj,
                                       const Header &hdr,
                                       const AbstractNodeInfo *sender) {

    ErrorCodes::Code result = ErrorCodes::InternalError;

    auto request = obj.dynamicCast<Request>();

    if (!request) {
        return false;
    }

    if (request->getRequestCmd() == static_cast<quint8>(PKG::UserRequestType::LogIn)) {
        result = loginUser(*obj.staticCast<PKG::UserMember>().data(), sender);
    } else if (request->getRequestCmd() == static_cast<quint8>(PKG::UserRequestType::SignUp)) {
        result = registerNewUser(*obj, sender);
    } else if (request->getRequestCmd() == static_cast<quint8>(PKG::UserRequestType::LogOut)) {
        result = logOutUser(*obj, sender);
    } else if (request->getRequestCmd() == static_cast<quint8>(PKG::UserRequestType::Remove)) {

        auto requesterId = getSender(sender, obj.data());
        if (deleteObject(requesterId, obj) != DBOperationResult::Allowed) {
            prepareAndSendBadRequest(sender->networkAddress(), hdr,
                                     ErrorCodes::OperatioForbiden, REQUEST_ERROR);
        }
    }

    if (result == ErrorCodes::InternalError) {
        QuasarAppUtils::Params::log("Internal error ocured in the loginUser or registerNewUser method.",
                                    QuasarAppUtils::Error);
        prepareAndSendBadRequest(sender->networkAddress(), hdr,
                                 result, REQUEST_INTERNAL_ERROR);

        return false;
    }

    if (result != ErrorCodes::NoError) {
        prepareAndSendBadRequest(sender->networkAddress(), hdr,
                                 result, REQUEST_INTERNAL_ERROR);
    }

    return true;
}

void SingleServer::prepareAndSendBadRequest(const HostAddress& address,
                                            const Header& lastHeader,
                                            unsigned char error,
                                            int punishment) {

    badRequest(address, lastHeader,
               {
                   error,
                   ErrorCodes::DBErrorCodesHelper::toString(error)
               },
               punishment);

}

QStringList SingleServer::SQLSources() const {
    return {":/sql/DataBaseSpace/Res/UserDB.sql"};
}

}
