/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "singleserver.h"

#include <authrequest.h>
#include <sqldbcache.h>
#include <basenodeinfo.h>
#include <badrequest.h>

namespace QH {

SingleServer::SingleServer()
{

}

UserOperationResult SingleServer::registerNewUser(PKG::UserMember user,
                                                 const AbstractNodeInfo *info) {
    if (!db()) {
        return UserOperationResult::InternalError;
    }

    auto localObject = db()->getObject(user);

    if (localObject) {
        return UserOperationResult::UserExits;
    }

    user.setAuthenticationData(hashgenerator(user.authenticationData()));

    auto requester = getSender(info, &user);

    addUpdatePermission(requester, user.dbAddress(), Permission::Write);

    if (!db()->insertObject(QSharedPointer<decltype(user)>::create(user))) {
        return UserOperationResult::InternalError;
    };

    return loginUser(user, info);
}

// To-Do fix me bug  #12 https://github.com/QuasarApp/Heart/issues/12
UserOperationResult SingleServer::loginUser(PKG::UserMember user,
                                           const AbstractNodeInfo *info) {

    if (!db()) {
        return UserOperationResult::InternalError;
    }

    auto localObject = db()->getObject(user);

    if (!localObject) {
        return UserOperationResult::UserNotExits;
    }

    if (localObject->authenticationData() != hashgenerator(user.authenticationData())) {
        return UserOperationResult::UserInvalidPasswoed;
    }

    auto nodeinfo = dynamic_cast<const BaseNodeInfo*>(info);
    if (!nodeinfo)
        return UserOperationResult::InternalError;

    AccessToken token((nodeinfo->token().toBytes()));

    if (token.isValid()) {
        return UserOperationResult::UserAlreadyLogged;
    }

    token = localObject->token();

    if (!token.isValid()) {
        token = generateToken(AccessToken::Year);
    }

    auto editableNodeInfo = static_cast<BaseNodeInfo*>(getInfoPtr(info->networkAddress()));
    if (!editableNodeInfo)
        return UserOperationResult::InternalError;

    editableNodeInfo->setToken(token);
    user.setToken(token);
    user.setAuthenticationData("");

    if (!sendData(&user, info->networkAddress())) {
        return UserOperationResult::InternalError;
    };

    return UserOperationResult::Success;
}

UserOperationResult SingleServer::loginOutUser(PKG::UserMember user,
                                              const AbstractNodeInfo *info) {

    if (!db()) {
        return UserOperationResult::InternalError;
    }

    auto localObject = db()->getObject(user);

    if (!localObject) {
        return UserOperationResult::UserNotExits;
    }

    auto nodeinfo = dynamic_cast<const BaseNodeInfo*>(info);
    if (!nodeinfo)
        return UserOperationResult::InternalError;

    AccessToken token((nodeinfo->token().toBytes()));
    if (!token.isValid()) {
        return UserOperationResult::UserNotLogged;
    }

    if (user.token() != token) {
        return UserOperationResult::UserNotLogged;
    }

    auto mutableNodeInfo = dynamic_cast<BaseNodeInfo*>(getInfoPtr(info->networkAddress()));
    if (!mutableNodeInfo)
        return UserOperationResult::InternalError;

    mutableNodeInfo->setToken(AccessToken{});

    return UserOperationResult::Success;

}

AccessToken SingleServer::generateToken(int length) {
    return AccessToken(length);
}

ParserResult SingleServer::parsePackage(const Package &pkg, const AbstractNodeInfo *sender) {
    auto parentResult = DataBaseNode::parsePackage(pkg, sender);
    if (parentResult != QH::ParserResult::NotProcessed) {
        return parentResult;
    }

    if (H_16<QH::PKG::AuthRequest>() == pkg.hdr.command) {
            auto obj = QSharedPointer<QH::PKG::AuthRequest>::create(pkg);

            if (!obj->isValid()) {
                badRequest(sender->networkAddress(), pkg.hdr,
                           {
                               ErrorCodes::InvalidRequest,
                               "AuthRequest is invalid"
                           });
                return ParserResult::Error;
            }

            if (!workWithUserRequest(obj, pkg, sender)) {
                return QH::ParserResult::Error;
            }

            return QH::ParserResult::Processed;

        }

    return QH::ParserResult::NotProcessed;

}

QByteArray SingleServer::hashgenerator(const QByteArray &data) {
    return DataBaseNode::hashgenerator(data + "singelserversoult");
}

bool SingleServer::workWithUserRequest(const QSharedPointer<PKG::UserMember>& obj,
                                       const Package &pkg,
                                       const AbstractNodeInfo *sender) {

    UserOperationResult result = UserOperationResult::InternalError;

    auto request = obj.dynamicCast<Request>();

    if (!request) {
        return false;
    }

    if (request->getRequestCmd() == static_cast<quint8>(PKG::UserRequestType::LogIn)) {
        result = loginUser(*obj.staticCast<PKG::UserMember>().data(), sender);
    } else if (request->getRequestCmd() == static_cast<quint8>(PKG::UserRequestType::SignUp)) {
        result = registerNewUser(*obj, sender);
    } else if (request->getRequestCmd() == static_cast<quint8>(PKG::UserRequestType::LogOut)) {
        result = loginOutUser(*obj, sender);
    } else if (request->getRequestCmd() == static_cast<quint8>(PKG::UserRequestType::Remove)) {

        auto requesterId = getSender(sender, obj.data());

        if (requesterId && deleteObject(requesterId, obj) != DBOperationResult::Allowed) {
            badRequest(sender->networkAddress(), pkg.hdr,
                       {
                           ErrorCodes::OperatioForbiden,
                          " Permission denied"
                       });
        }
    }

    switch (result) {

    case UserOperationResult::InternalError: {
        QuasarAppUtils::Params::log("Internal error ocured in the loginUser or registerNewUser method.",
                                    QuasarAppUtils::Error);
        badRequest(sender->networkAddress(), pkg.hdr,
                   {
                       ErrorCodes::InternalError,
                      "Internal server error."
                      " Please create issue about this problem in the support page "
                      " https://github.com/QuasarApp/Heart/issues/new"
                   },
                   REQUEST_INTERNAL_ERROR);

        return false;
    }

    case UserOperationResult::UserExits: {
        badRequest(sender->networkAddress(), pkg.hdr,
                   {
                       ErrorCodes::InternalError,
                       "Such user already exists "
                   },
                   REQUEST_LOGIN_ERROR);
        return true;

    }

    case UserOperationResult::UserNotExits: {
        badRequest(sender->networkAddress(), pkg.hdr,
                   {
                       ErrorCodes::UserNotExits,
                       "Such user not exists "
                   },
                   REQUEST_LOGIN_ERROR);
        return true;

    }

    case UserOperationResult::UserInvalidPasswoed: {
        badRequest(sender->networkAddress(), pkg.hdr,
                   {
                       ErrorCodes::UserInvalidPasswoed,
                       "Invalid password "
                   },
                   REQUEST_LOGIN_ERROR);
        return true;

    }

    case UserOperationResult::UserAlreadyLogged: {
        badRequest(sender->networkAddress(), pkg.hdr,
                   {
                       ErrorCodes::UserInvalidPasswoed,
                       "User Already Logged"
                   },
                   REQUEST_LOGIN_ERROR);
        return true;

    }

    case UserOperationResult::UserNotLogged: {
        badRequest(sender->networkAddress(), pkg.hdr,
                   {
                       ErrorCodes::UserInvalidPasswoed,
                       "User is not Logged"
                   },
                   REQUEST_LOGIN_ERROR);
        return true;

    }

    case UserOperationResult::Success: {
        return true;
    }
    }

    badRequest(sender->networkAddress(), pkg.hdr,
               {
                   ErrorCodes::InternalError,
                  "Internal server error."
                  " Please create issue about this problem in the support page "
                  " https://github.com/QuasarApp/Heart/issues/new"
               },
               REQUEST_INTERNAL_ERROR);
    return false;
}

}
